"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMB11_DFP\2.2.181\tools\samb_makeimage.bat" "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMB11_DFP\2.2.181\patches\patch.img" "%1.bin"  app_entry _sstack _estack _sfixed
"C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\SAMB11_DFP\2.2.181\tools\SambImageCreator.exe" -f "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMB11_DFP\2.2.181\patches\patch.img" -a "%1.bin" -o flash.img -r 0x10008000 -s 0x10014bc8 -e 0x100153c8 -i 0x10008000