import sys
import subprocess
import importlib.util as importlib_util

class PythonConfiguration:
    @classmethod
    def Validate(cls, cla: bool):
        if not cls.__ValidatePython() and not cla:
            return # cannot validate further

        for packageName in ["requests"]:
            if not cls.__ValidatePackage(packageName, cla):
                return # cannot validate further

    @classmethod
    def __ValidatePython(cls, versionMajor = 3, versionMinor = 3):
        if sys.version is not None:
            print("Python version {0:d}.{1:d}.{2:d} detected.".format( \
                sys.version_info.major, sys.version_info.minor, sys.version_info.micro))
            if sys.version_info.major < versionMajor or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
                print("Python version too low, expected version {0:d}.{1:d} or higher.".format( \
                    versionMajor, versionMinor))
                return False
            return True

    @classmethod
    def __ValidatePackage(cls, packageName, cla: bool):
        if importlib_util.find_spec(packageName) is None:
            return cls.__InstallPackage(packageName, cla)
        return True

    @classmethod
    def __InstallPackage(cls, packageName, cla: bool):
        permissionGranted = cla
        while not permissionGranted:
            reply = str(input("Would you like to install Python package '{0:s}'? [Y/N]: ".format(packageName))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')
        
        print(f"Installing {packageName} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

        return cls.__ValidatePackage(packageName)

if __name__ == "__main__":
    PythonConfiguration.Validate()
