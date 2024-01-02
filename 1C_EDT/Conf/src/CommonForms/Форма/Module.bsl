&НаКлиенте
Перем ТестоваяКомпонента;

&НаКлиенте
Перем АдресУстройства;

&НаКлиенте
Процедура ПриОткрытии(Отказ)
	Элементы.Соеденить.Доступность = Ложь;	
	Элементы.Отключить.Доступность = Ложь;
	Элементы.Состояние.Доступность = Ложь;		
КонецПроцедуры

&НаКлиенте
Процедура Подключить(Команда)
	АдресУстройства = "";

	КомпонентаПодключена = ПодключитьВнешнююКомпоненту("ОбщийМакет.BarcodeScanner", "BTScaner", ТипВнешнейКомпоненты.Native);
	Если Не КомпонентаПодключена Тогда
		Сообщить("Ошибка подключения BTScaner");	
		Возврат;
	КонецЕсли;  
	 
	Попытка
		ТестоваяКомпонента = Новый("AddIn.BTScaner.BTScaner");  
	Исключение
		Сообщить("Ошибка класса AddIn.BTScaner.BTScaner");	
	КонецПопытки;
	
	
	Если НЕ ТестоваяКомпонента = Неопределено Тогда
		ОписаниеУстройства = ТестоваяКомпонента.ОписаниеУстройства; 
		Элементы.ДекорацияИнформация.Заголовок = ОписаниеУстройства +", укажите BT устройство:"; 									

		ПолучитьУстройства()
	КонецЕсли;
КонецПроцедуры



&НаКлиенте
Процедура ПолучитьУстройства()
	СтроукаУстройства = ТестоваяКомпонента.СписокУстройств();
	МассивУстройств = СтрРазделить(СтроукаУстройства, Символы.ПС);
	
	Элементы.Устройство.СписокВыбора.Очистить();
	АдресУстройства = "";
	Для Каждого ОписаниеУстройства Из МассивУстройств цикл
		Адрес = ПолучитьАдрес(ОписаниеУстройства);
		Если Адрес = "" Тогда
			Продолжить;
		КонецЕсли;
		
		Элементы.Устройство.СписокВыбора.Добавить(Адрес,ОписаниеУстройства);
	КонецЦикла;
КонецПроцедуры

&НаКлиенте
Функция ПолучитьАдрес(ОписаниеУстройства)
	
	Позиция = СтрНайти(ОписаниеУстройства, "|");
	Если Позиция = 0 Тогда 
		возврат "";
	КонецЕсли;

	Возврат Лев(ОписаниеУстройства,Позиция-1);
КонецФункции

&НаКлиенте
Процедура УстройствоОбработкаВыбора(Элемент, ВыбранноеЗначение, СтандартнаяОбработка)
	АдресУстройства = ВыбранноеЗначение;
КонецПроцедуры

&НаКлиенте
Процедура УстройствоПриИзменении(Элемент)
	Если ЗначениеЗаполнено(АдресУстройства) Тогда
		Элементы.Соеденить.Доступность = Истина;	
		Элементы.Отключить.Доступность = Истина;
		Элементы.Состояние.Доступность = Истина;	
	Иначе
		Элементы.Соеденить.Доступность = Ложь;	
		Элементы.Отключить.Доступность = Ложь;	
		Элементы.Состояние.Доступность = Ложь;		
	КонецЕсли;
КонецПроцедуры

&НаКлиенте
Процедура Соеденить(Команда)
	ТестоваяКомпонента.ИнициализироватьУстройство(АдресУстройства); 
	РезультатыСканирования = РезультатыСканирования + "Подключено: " + АдресУстройства + Символы.ПС;
КонецПроцедуры

&НаКлиенте
Процедура Отключить(Команда)
	ТестоваяКомпонента.ОстановитьУстройство(); 
	РезультатыСканирования = РезультатыСканирования + "Устройство отключено"+ Символы.ПС;
КонецПроцедуры

&НаКлиенте
Процедура СостояниеУстройства(Команда)
	Если ТестоваяКомпонента.Доступно() Тогда
		РезультатыСканирования = РезультатыСканирования + "Доступно" + Символы.ПС;	
	Иначе
		РезультатыСканирования = РезультатыСканирования + "Недоступно" + Символы.ПС;	
	КонецЕсли; 
КонецПроцедуры


&НаКлиенте
Процедура ВнешнееСобытие(Источник, Событие, Данные)
    Если Источник = "ru.bootcode.btscaner" И Событие = "barcode" Тогда
       РезультатыСканирования = РезультатыСканирования + Данные + Символы.ПС;
    КонецЕсли;
    
     Если Источник = "ru.bootcode.btscaner" И Событие = "cancel" Тогда
       РезультатыСканирования = РезультатыСканирования + "Потеря соединения!" + Символы.ПС;
       ТестоваяКомпонента.ОстановитьУстройство();
    КонецЕсли;
КонецПроцедуры


