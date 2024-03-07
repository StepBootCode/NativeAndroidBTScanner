ECHO ON


copy "BTScanerNative\ARM\Release\BTScaner_ARM.so" "Release\Test"
copy "BTScanerNative\ARM64\Release\BTScaner_ARM64.so" "Release\Test"
copy "BTScanerNative\x86\Release\BTScaner_x86.so" "Release\Test"
copy "BTScanerNative\x64\Release\BTScaner_x64.so" "Release\Test"


copy "androidTest\app\build\outputs\apk\debug\ru_bootcode_btscaner-debug.apk" "Release\Test\ru_bootcode_btscaner.apk"

copy "Release\ANDROID_MANIFEST_EXTENTIONS.XML" "Release\Test\ANDROID_MANIFEST_EXTENTIONS.XML"
copy "Release\MANIFEST.XML" "Release\Test\MANIFEST.XML"

cd Release
cd Test

del outputTest.zip
tar.exe -a -c -f outputTest.zip ru_bootcode_btscaner.apk ANDROID_MANIFEST_EXTENTIONS.XML MANIFEST.XML BTScaner_ARM.so BTScaner_ARM64.so BTScaner_x64.so BTScaner_x86.so

Pause