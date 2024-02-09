ECHO ON


copy "BTScaner\ARM\Release\BTScaner_ARM.so" "Release"
copy "BTScaner\ARM64\Release\BTScaner_ARM64.so" "Release"
copy "BTScaner\x86\Release\BTScaner_x86.so" "Release"
copy "BTScaner\x64\Release\BTScaner_x64.so" "Release"


copy "androidOnlyDriver\app\build\outputs\apk\debug\ru_bootcode_btscaner-debug.apk" "Release\ru_bootcode_btscaner.apk"

cd Release
del outputOnlyDriver.zip
tar.exe -a -c -f outputOnlyDriver.zip ru_bootcode_btscaner.apk INFO.XML ANDROID_MANIFEST_EXTENTIONS.XML MANIFEST.XML BTScaner_ARM.so BTScaner_ARM64.so BTScaner_x64.so BTScaner_x86.so

Pause