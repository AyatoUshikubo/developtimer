#using <C:\Windows\Microsoft.NET\assembly\GAC_MSIL\System.Speech\v4.0_4.0.0.0__31bf3856ad364e35\System.Speech.dll>
#include<stdlib.h>
#include<stdio.h>
using namespace System;
using namespace System::Speech::Synthesis;

// Written by Aki
// To compile
// cl /clr speechsynthesizer.cpp


void main(int argc, char *argv[]){
	
	
	char texts[10000];

	if(argc==1){
		printf("usage: sayZira something to speech\n");
		sprintf(texts,"Write some texts as command line parameters");
	}
	else{
		int n=0;
		for(int i=1; i<argc; i++){
			n+=sprintf(texts+n, "%s ", argv[i]);
		}
	}
	printf("texts = %s\n", texts);
	
	SpeechSynthesizer^ synth = gcnew SpeechSynthesizer();
	synth->SelectVoice("Microsoft Zira Desktop");
	String^ p = gcnew String(texts);
	synth->Speak(p);
}
