Внешняя компонента 1С: Предприятие для Android.  
Компонента позволяет получить и подключить устройсва типа Сканер Штрихкодов по протоколу Bluetooth SPP.  
  
Компонента компилировалась и использовалась со следующим ПО:  
Visual Studio 2022  
Android Studio Dolphin | 2021.3.1  
1С: Предприятие. Мобильная платформа 8.3.22.57  

Катлоги и файлы:   
android -  Приложение на java, для Android Studio  
include, jni - Общие заголовочные файлы из шаблонов 1С  
BTScaner - Содержит файлы для Visual Studio, содержит файл проекта *.sln  
Release - Содержит скомпилированые файлы после запуска BuildRealise.cmd  
1Cv8.cf - Конфигурация мобильного приложения 1С: Предприятие.  
  
После компиляции и запуска BuildRealise.cmd, каталог Release будет содержать ZIP файл с внешней компонентой.  
  
p.s. Пришлось отказаться от BroadcastReceiver и ACTION_ACL_DISCONNECTED, событие может не возникать на некоторых устройствах с задержкой до 30сек. или вовсе не возникать 
