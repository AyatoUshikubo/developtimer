//dev
#include"headers.h"

#include<TDatime.h>
#include<time.h>
#include<TTimer.h>
#include<TText.h>
#include<TSystem.h>

//Windows:"Zira"
//Mac:" -v Speaker" Speaker = Alex(US)Daniel(GB)Fiona(scotland)Fred(US)Karen(AU)Moira(IE)Rishi(IN)Samantha(US)Tessa(ZA)Veena(IN)Victoria(US) Kyoko(JP)
TString who_say = " -v Samantha" ;

int timer_right = 150;

//time difference(hour)
int timedif = 0;

//縦幅
int HorizontalSize = 860;

int button_width = 45;
int button_hight = 20;
int rightbutton = 0;
int centerbutton = (timer_right-button_width)/2;
int leftbutton = timer_right-button_width;

//the number of Chain
int Chain_num = 9;

//skip(second)
int time_skip = 0;

//作業ごとの時間(min)
int devtime_min = 20;
int stoptime_min = 10;
int fixtime_min = 60;
int washtime_min = 60;

//elapsed time(second)
int devtime = devtime_min*60;
int stoptime = stoptime_min*60+devtime;
int fixtime = fixtime_min*60+stoptime;
int washtime = washtime_min*60+fixtime;

//start time(min)
int devmin  = 0;
int stopmin = devmin+devtime_min;
int fixmin  = stopmin+stoptime_min;
int washmin = fixmin+fixtime_min;
int finishmin = washmin+washtime_min;

class Event: public TObject{
	
	private :
	int fMin;
	int fSec;
	TString fMessage;
	bool fDone;
	
	public:
	Event(int min, int sec, TString message): fMin(min), fSec(sec), fMessage(message), fDone(kFALSE){}
	~Event(){}
	
	void Check(time_t t){
		if(fDone) return;
		if(t>=fMin*60+fSec){
			TString cmd = "say"+who_say+" \""+fMessage+"\""+" &"; 
			printf("%s\n", cmd.Data());
			gSystem->Exec(cmd);
			fDone=kTRUE;
		}
	}
	
	
	int RemainingTime(time_t t){
		return fMin*60+fSec-t;
	}
	
	int EventTime(){
		return fMin*60+fSec;
	}
	
	void Reset() { fDone=kFALSE;}
	
	const char *GetMessage(){return fMessage.Data();}
};

class DevTimer : public TGHorizontalFrame{
	private:
	TGLabel *fLabelChain;
	TGLabel *fLabel2;
	TTimer *timer;
	FILE *fp;
	bool skip_if = false;
	bool reset_if = false;
	time_t t0, t, t1, t2, total;
	int dt;
	int sh, sm, ss, start;
	TDatime T0;
	int ID;
	TCanvas *c1,*c2;
	TText *text;
	TObjArray *fMsgEvents;
	TObjArray *fTimeEvents;
	TGTextButton *fb1 = new TGTextButton(this,"Start");
	// TGTextButton *fb3 = new TGTextButton(this,"Resume");
	// TGTextButton *fb2 = new TGTextButton(this,"Stop");
	TGTextButton *fb4 = new TGTextButton(this,"Skip");
	TGTextButton *fb5 = new TGTextButton(this,"Unlock");
	TGTextButton *fb6 = new TGTextButton(this,"Reset");
	TGTextButton *fb7 = new TGTextButton(this,"Lock");
	TGTextButton *fb8 = new TGTextButton(this,"Set0");
	public:
	DevTimer(TGMainFrame *fMainFrame, int id);
	~DevTimer(){if(timer) timer->TurnOff();};
	void OnTimer();
	void Start();
	void Stop();
	void Resume();
	void Skip();
	void Reset();
	void Unlock();
	void Lock();
	void Set0();
	void SpeechTime(time_t t);
	ClassDef(DevTimer,0) // Development 
};
// class内関数の定義(はじまり)
void DevTimer::SpeechTime(time_t t){
	char buf[256];
	sprintf(buf, "say"+who_say+" %02d:%02d:%02d", T0.GetHour(), T0.GetMinute(), T0.GetSecond());
	gSystem->Exec(buf);
}

void DevTimer::Start(){
   	time(&t0);
	// printf("%ld\n",(t0+9*60*60)/(24*60*60));
	// printf("%d\n",start);
	if(skip_if){t0 = start;}
   	T0.Set(t0);
   	fLabel2->SetText(Form(" Start at %02d:%02d:%02d", (T0.GetHour()+timedif)%24, T0.GetMinute(), T0.GetSecond()));

	for(int i=0;i<fTimeEvents->GetEntries();i++){
		Event *ev = (Event *)fTimeEvents->At(i);
		ev->Reset();
	}
	for(int i=0;i<fMsgEvents->GetEntries();i++){
		Event *ev = (Event *)fMsgEvents->At(i);
		ev->Reset();
	}
	
	timer->TurnOn();

	TString id = Form("%d",ID);
	TString Date  =	Form("%d",T0.GetDate());
	TString Hour  =	Form("%02d",T0.GetHour());
	TString Minute=	Form("%02d",T0.GetMinute());
	TString Second=	Form("%02d",T0.GetSecond());

   	// SpeechTime(t0);
	if(!skip_if){
		remove("Log/Chain"+id+".log");
		fp = fopen("Log/Chain"+id+".log", "at");
   		// FILE *fp = fopen("Log/Chain"+id+"/"+Hour+Minute+Second+","+Date+".log", "at");
		// FILE *fp = fopen("develop.log", "at");
   		// fprintf(fp, "Chain %2d, Start at %02d:%02d:%02d\n", ID, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
		fprintf(fp, "%ld %02d %02d %02d\n", t0, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
		fclose(fp);
	}
	
   	fprintf(stdout, "Chain %2d, Start at %02d:%02d:%02d\n", ID, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
   	
	skip_if = false;
	reset_if = false;

	fb1->SetState(kButtonDisabled);
	fb4->SetState(kButtonDisabled);
	fb5->SetEnabled();

}

void DevTimer::Stop(){
	timer->TurnOff();
	// timer->Remove();
}

void DevTimer::Resume(){
	timer->TurnOn();
}

void DevTimer::Skip(){
	TString id = Form("%d",ID);
	fp = fopen("Log/Chain"+id+".log","r");
	fscanf(fp,"%d %02d %02d %02d",&start,&sh,&sm,&ss);
	skip_if = true;
	// printf("%d:%d:%d\n",sh,sm,ss);
	fclose(fp);

	fb4->SetState(kButtonDisabled);
	fb8->SetEnabled();
}

void DevTimer::Reset(){
	fb1->SetEnabled();
	fb4->SetEnabled();
	fb5->SetState(kButtonDisabled);
	fb6->SetState(kButtonDisabled);
	fb7->SetState(kButtonDisabled);
	fb8->SetState(kButtonDisabled);

	// skip_if = false;
	reset_if = true;

	fLabel2->SetText(" Start at - - : - - : - -  ");
}

void DevTimer::Unlock(){
	fb5->SetState(kButtonDisabled);
	fb6->SetEnabled();
	fb7->SetEnabled();
	// fb5->ChangeText("Lock");
	// fb5->Connect("Clicked()", "DevTimer", this, "Lock()");
}

void DevTimer::Lock(){
	// fb5->Connect("Clicked()", "DevTimer", this, "Unlock()");
	// fb5->ChangeText("Unlock");
	fb5->SetEnabled();
	fb6->SetState(kButtonDisabled);
	fb7->SetState(kButtonDisabled);
}

void DevTimer::Set0(){
	skip_if = false;
	fb4->SetEnabled();
	fb8->SetState(kButtonDisabled);
}

void DevTimer::OnTimer(){
	time(&t);
	if(reset_if){t = t0;}
	dt= t-t0+time_skip;
	// if(reset_if){dt = washtime;}
	int h=dt/3600;
	int m=(dt%3600)/60;
	int s=dt%60;
//	fLabelChain->SetText(Form("%02d:%02d:%02d", h,m,s));
	c1->cd();
	c1->Clear();
	c1->Divide(2,1);
	c1->cd(1);
	text->SetTextSize(1.0);
	text->SetTextColor(kRed);
	if(reset_if){text->DrawTextNDC(0.02,0.1, " ");}
	else
	{
	if(dt<washtime){
		text->DrawTextNDC(0.02,0.1, Form("%02d:%02d:%02d", h,m,s));
	}
	else if(dt>=washtime){
		// if(!reset_if){
		text->DrawTextNDC(0.02,0.1, "Finished");
		// }
		// else{
		// 	text->DrawTextNDC(0.02,0.1, "    ");
		// }
	}
	text->SetTextSize(0.5);
	c1->cd(2);
	Event *ev_current = NULL;
	Event *ev_next = NULL;
	for(int i=0;i<fTimeEvents->GetEntries();i++){
		Event *ev = (Event *)fTimeEvents->At(i);
		int dt2 = ev->RemainingTime(dt);
		int evt = ev->EventTime();
		int dt3 = evt - dt2;
		if(dt2>0){
			int h2=dt2/3600;
			int m2=(dt2%3600)/60;
			int s2=dt2%60;
			text->DrawTextNDC(0.02,0.1,Form("%02d:%02d:%02d to %s", h2,m2,s2,ev->GetMessage()));
			ev_next = ev;
			break;
		}


		ev_current = ev;
	}
	if(NULL==ev_next) {
		int dt2 = -ev_current->RemainingTime(dt);
		int h2=dt2/3600;
		int m2=(dt2%3600)/60;
		int s2=dt2%60;
		if(dt<washtime){
			text->DrawTextNDC(0.02,0.1,Form("%02d:%02d:%02d Finished", h2,m2,s2));
		}
		else if(dt>=washtime){
			DevTimer::Stop();
		}
	}

	//行程ごとの時間の描画
	if(dt<devtime){
		text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %sing", h,m,s, ev_current->GetMessage()));
	}
	else if(dt<stoptime){
		int dt3 = dt - devtime;
		int h3=dt3/3600;
		int m3=(dt3%3600)/60;
		int s3=dt3%60;
		text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %sing", h3,m3,s3, ev_current->GetMessage()));
	}
	else if(dt<fixtime){
		int dt3 = dt - stoptime;
		int h3=dt3/3600;
		int m3=(dt3%3600)/60;
		int s3=dt3%60;
		text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %sing", h3,m3,s3, ev_current->GetMessage()));
	}
	else if(dt<washtime){
		int dt3 = washtime_min*60- dt + fixtime;
		int h3=dt3/3600;
		int m3=(dt3%3600)/60;
		int s3=dt3%60;
		text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %sing", h3,m3,s3, ev_current->GetMessage()));
	}
	else if(dt>=washtime){
		DevTimer::Stop();
	}
	}
	

//	text->Draw();
	c1->Modified();
	c1->Update();
	
	for(int i=0;i<fMsgEvents->GetEntries();i++){
		Event *ev = (Event *)fMsgEvents->At(i);
		ev->Check(dt);
	}
}
//class内関数の定義(終わり)

DevTimer::DevTimer(TGMainFrame *fMainFrame, int id) : TGHorizontalFrame(fMainFrame,200,HorizontalSize/Chain_num,kHorizontalFrame| kRaisedFrame), ID(id){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
	fTimeEvents = new TObjArray;
	fTimeEvents->Add( new Event(devmin,   0, "Develop"));
	fTimeEvents->Add( new Event(stopmin,  0, "Stop"));
	fTimeEvents->Add( new Event(fixmin,   0, "Fix"));
	fTimeEvents->Add( new Event(washmin,  0, "Wash"));

	fMsgEvents = new TObjArray;

	TString chain   = Form("Chain %d. ",id);
	TString in1min  = "in one minute.";
	TString in3min  = "in three minutes.";
	TString in30sec = "in thirty seconds.";
	TString in10sec = "in ten seconds.";
	TString in5sec = "five";
	TString in4sec = "four";
	TString in3sec = "three";
	TString in2sec = "two";
	TString in1sec = "one";
	TString past30sec = "thirty seconds passed";
	TString past1min  = "one minute passed";
	TString start   = "start ";

	TString dev  = "development ";
	TString stop = "stop ";
	TString fix  = "fix ";
	TString wash = "wash ";

	TString finished = "finished ";

	// ���b�Z�[�W�@�o�^
	fMsgEvents->Add( new Event(devmin,   0, start+dev+chain));
	// fMsgEvents->Add( new Event(devmin,  30, past30sec));
//	fMsgEvents->Add( new Event(devmin,   5*60, TString("�ӂ����Ԃ݂ɂ���")));
//	fMsgEvents->Add( new Event(devmin,  10*60, TString("�Ă�݂ɂ���")));
//	fMsgEvents->Add( new Event(devmin,  15*60, TString("�ӂ��ӂĂ��[��݂ɂ���")));
//	fMsgEvents->Add( new Event(devmin,  20*60, TString("�Ƃ�����Ă��݂ɂ���")));

	fMsgEvents->Add( new Event(stopmin-3, 0, chain+stop+in3min));
	fMsgEvents->Add( new Event(stopmin, -60, chain+stop+in1min));
	fMsgEvents->Add( new Event(stopmin, -30, chain+stop+in30sec));
	fMsgEvents->Add( new Event(stopmin, -10, in10sec));
	fMsgEvents->Add( new Event(stopmin,  -5, in5sec));
	fMsgEvents->Add( new Event(stopmin,  -4, in4sec));
	fMsgEvents->Add( new Event(stopmin,  -3, in3sec));
	fMsgEvents->Add( new Event(stopmin,  -2, in2sec));
	fMsgEvents->Add( new Event(stopmin,  -1, in1sec));
	fMsgEvents->Add( new Event(stopmin,   0, start+stop+chain));
	// fMsgEvents->Add( new Event(stopmin,  30, past30sec));
	// fMsgEvents->Add( new Event(stopmin,  60, past1min));

	fMsgEvents->Add( new Event(fixmin-3, 0, chain+fix+in3min));
	fMsgEvents->Add( new Event(fixmin, -60, chain+fix+in1min));
	fMsgEvents->Add( new Event(fixmin, -30, chain+fix+in30sec));
	fMsgEvents->Add( new Event(fixmin, -10, in10sec));
	fMsgEvents->Add( new Event(fixmin,  -5, in5sec));
	fMsgEvents->Add( new Event(fixmin,  -4, in4sec));
	fMsgEvents->Add( new Event(fixmin,  -3, in3sec));
	fMsgEvents->Add( new Event(fixmin,  -2, in2sec));
	fMsgEvents->Add( new Event(fixmin,  -1, in1sec));
	fMsgEvents->Add( new Event(fixmin,   0, start+fix+chain));
	// fMsgEvents->Add( new Event(fixmin,  30, past30sec));
	// fMsgEvents->Add( new Event(fixmin,  60, past1min));

	fMsgEvents->Add( new Event(washmin-3, 0, chain+wash+in3min));
	fMsgEvents->Add( new Event(washmin, -60, chain+wash+in1min));
	fMsgEvents->Add( new Event(washmin, -30, chain+wash+in30sec));
	fMsgEvents->Add( new Event(washmin, -10, in10sec));
	fMsgEvents->Add( new Event(washmin,  -5, in5sec));
	fMsgEvents->Add( new Event(washmin,  -4, in4sec));
	fMsgEvents->Add( new Event(washmin,  -3, in3sec));
	fMsgEvents->Add( new Event(washmin,  -2, in2sec));
	fMsgEvents->Add( new Event(washmin,  -1, in1sec));
	fMsgEvents->Add( new Event(washmin,   0, start+wash+chain));
	
	fMsgEvents->Add( new Event(finishmin, 0, chain+finished));
   
   
   	timer = new TTimer(1000);
   	timer->Connect("Timeout()","DevTimer", this, "OnTimer()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   	fLabelChain = new TGLabel(this,Form("Chain %d", id));
   	fLabelChain->SetTextJustify(36);
   	fLabelChain->SetMargins(0,0,0,0);
   	fLabelChain->SetWrapLength(-1);
   	fLabelChain->Resize(100,80);
   	fLabelChain->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

   	AddFrame(fLabelChain, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fLabelChain->MoveResize(3,3,100,20);

   	fLabel2 = new TGLabel(this," Start at - - : - - : - -  ");
   	fLabel2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

   	fLabel2->SetTextJustify(36);
   	fLabel2->SetMargins(0,0,0,0);
   	fLabel2->SetWrapLength(-1);
   	AddFrame(fLabel2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fLabel2->MoveResize(3,25,120,20);

   	//Button
   	fb1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb1->Connect("Clicked()", "DevTimer", this, "Start()");
   	fb1->Resize(71,24);
   	AddFrame(fb1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb1->MoveResize(rightbutton,50,button_width,button_hight);

   	// fb2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	// fb2->Connect("Clicked()", "DevTimer", this, "Stop()");
   	// fb2->Resize(71,24);
   	// AddFrame(fb2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	// fb2->MoveResize(10,80,50,20);

   	// fb3->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	// fb3->Connect("Clicked()", "DevTimer", this, "Resume()");
   	// fb3->Resize(71,24);
   	// AddFrame(fb3, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	// fb3->MoveResize(65,80,50,20);

   	fb4->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb4->Connect("Clicked()", "DevTimer", this, "Skip()");
   	fb4->Resize(71,24);
   	AddFrame(fb4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb4->MoveResize(centerbutton,50,button_width,button_hight);

	fb5->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb5->Connect("Clicked()", "DevTimer", this, "Unlock()");
   	fb5->Resize(71,24);
   	AddFrame(fb5, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb5->MoveResize(centerbutton,70,button_width,button_hight);
	fb5->SetState(kButtonDisabled);

	fb6->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb6->Connect("Clicked()", "DevTimer", this, "Reset()");
   	fb6->Resize(71,24);
   	AddFrame(fb6, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb6->MoveResize(leftbutton,70,button_width,button_hight);
	fb6->SetState(kButtonDisabled);

	fb7->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb7->Connect("Clicked()", "DevTimer", this, "Lock()");
   	fb7->Resize(71,24);
   	AddFrame(fb7, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb7->MoveResize(rightbutton,70,button_width,button_hight);
	fb7->SetState(kButtonDisabled);

	fb8->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb8->Connect("Clicked()", "DevTimer", this, "Set0()");
   	fb8->Resize(71,24);
   	AddFrame(fb8, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb8->MoveResize(leftbutton,50,button_width,button_hight);
	fb8->SetState(kButtonDisabled);

   
    // embedded canvas
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c%d_1",ID), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    //Size of timer
   	fRootEmbeddedCanvas1->MoveResize(timer_right,0,900,95);
/*   
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas2 = new TRootEmbeddedCanvas(0,this,536,102);
   	Int_t wfRootEmbeddedCanvas2 = fRootEmbeddedCanvas2->GetCanvasWindowId();
   	c2 = new TCanvas(Form("c%d_2",ID), 10, 10, wfRootEmbeddedCanvas2);
   	fRootEmbeddedCanvas2->AdoptCanvas(c2);
   	AddFrame(fRootEmbeddedCanvas2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fRootEmbeddedCanvas2->MoveResize(513,3,380,100);  
*/
}

class DevTimers : public TGMainFrame {
	private:
	TObjArray eTimers;
	
	public:
	DevTimers();
	~DevTimers(){ for(int i=0; i<eTimers.GetEntries(); i++) delete eTimers.At(i);}
	
	ClassDef(DevTimers,0) // Development 
};

DevTimers::DevTimers() : TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame){
	// main frame
	SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	for(int i=1; i<=Chain_num; i++){
		DevTimer *t = new DevTimer((TGMainFrame *)this, i);
		AddFrame( t,  new TGLayoutHints(kLHintsExpandX | kLHintsTop,2,2,2,2));
		eTimers.Add(t);
	}

	SetMWMHints(kMWMDecorAll,
	                    kMWMFuncAll,
	                    kMWMInputModeless);
	MapSubwindows();

	Resize(GetDefaultSize());
	MapWindow();
	Resize(1000,570);
}


void developtimer()
{
	gStyle->SetCanvasColor(kBlack);
	new DevTimers;
}  
