ECHO ON


copy "BTScanerNative\ARM\Release\BTScaner_ARM.so" "Release\Native"
copy "BTScanerNative\ARM64\Release\BTScaner_ARM64.so" "Release\Native"
copy "BTScanerNative\x86\Release\BTScaner_x86.so" "Release\Native"
copy "BTScanerNative\x64\Release\BTScaner_x64.so" "Release\Native"


copy "androidNative\app\build\outputs\apk\debug\ru_bootcode_btscaner-debug.apk" "Release\Native\ru_bootcode_btscaner.apk"

copy "Release\ANDROID_MANIFEST_EXTENTIONS.XML" "Release\Native\ANDROID_MANIFEST_EXTENTIONS.XML"
copy "Release\MANIFEST.XML" "Release\Native\MANIFEST.XML"

cd Release
cd Native
del outputNative.zip
tar.exe -a -c -f output.zip ru_bootcode_btscaner.apk BTScaner_ARM.so BTScaner_ARM64.so BTScaner_x64.so BTScaner_x86.so ANDROID_MANIFEST_EXTENTIONS.XML MANIFEST.XML

Pause