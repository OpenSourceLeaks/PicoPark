AS3COMPILER:=asc2.jar

$?UNAME=$(shell uname -s)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?unixpath=$(shell cygpath -at unix $(1))
else
	$?nativepath=$(abspath $(1))
	$?unixpath=$(abspath $(1))
endif

ifneq (,$(findstring "asc2.jar","$(AS3COMPILER)"))
	$?AS3COMPILERARGS=java $(JVMARGS) -jar $(call nativepath,$(FLASCC)/usr/lib/$(AS3COMPILER)) -merge -md 
else
	echo "ASC is no longer supported" ; exit 1 ;
endif

T12: check

#ascファイル作成
	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		../as/*.as ../as/macro/*.as -outdir ../lib -out AGAL

#C++インタフェース自動生成
	java -jar $(call nativepath, $(FLASCC)/usr/lib/as3wig.jar) -i ../lib/AGAL.abc -o AGAL
	cp AGAL.h ../include/AGAL.h
	cp AGAL_internal.h ../include/AGAL_internal.h
	cp AGAL.cpp ../src/AGAL.cpp

#.aファイル生成
	"$(FLASCC)/usr/bin/g++" AGAL.cpp -c -emit-llvm &> AGAL.txt
	$(FLASCC)/usr/bin/ar rcs ../lib/libAGAL.a AGAL.o
	rm AGAL.* AGAL_*

check:
	@echo -------- build [$(TARGET)]-------- 
	@if [ -d $(FLASCC)/usr/bin ] ; then true ; \
	else echo "Couldn't locate FLASCC sdk directory, please invoke make with \"make FLASCC=/path/to/FLASCC/sdk ...\"" ; exit 1 ; \
	fi

	@if [ -d "$(FLEX)/bin" ] ; then true ; \
	else echo "Couldn't locate Flex sdk directory, please invoke make with \"make FLEX=/path/to/flex  ...\"" ; exit 1 ; \
	fi

clean:
	rm AGAL.* AGAL_* *.abc *.o
