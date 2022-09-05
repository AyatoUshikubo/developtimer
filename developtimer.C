//dev
#include"headers.h"

#include<TDatime.h>
#include<time.h>
#include<TTimer.h>
#include<TText.h>
#include<TSystem.h>

//Windows:"Zira"
//Mac:" -v Speaker" Speaker = Alex(US)Daniel(GB)Fiona(scotland)Fred(US)Karen(AU)Moira(IE)Rishi(IN)Samantha(US)Tessa(ZA)Veena(IN)Victoria(US) Kyoko(JP)
TString who_say = "Alex" ;

int timer_right = 150;

// setting file
FILE *fs;

TString day_of_week[7] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

int height_clock = 150;
int height_task = 200;
int height_setting = 300;

int width_mainframe = 950;
int width_subframe = 1418 - width_mainframe;
int width_setting = 200;

//縦幅
int HorizontalSize = 860;

int button_width = 45;
int button_height = 20;
int rightbutton = 0;
int centerbutton = (timer_right-button_width)/2;
int leftbutton = timer_right-button_width;

//the number of Chain
int Chain_num;

//time difference(hour)
int timedif = 0;

//skip(second)
int time_skip = (20*60-10)*1;

//配列に使うやつ
int k;

//作業ごとの時間(min)
int processtime_min[32];

//elapsed time(second)
int processtime[32];

//start time(min)
int processmin[32];

//process
char process[32][16];
TString process_ev[16];
TString process_current[16];

int dt2[16];
int dt2_compare[16];

class Task : public TGVerticalFrame{
	private:
	TText *text;
	TCanvas *c1;
	TTimer *timer;
	public:
	Task(TGHorizontalFrame *fMainFrame);
	~Task(){};
	void Compare();
	ClassDef(Task,0)
};

void Task::Compare(){
	int h = Chain_num/1.3;
	int Chain_id[16];
	for(int i=0;i<Chain_num;i++){
		dt2_compare[i] = dt2[i];
		Chain_id[i] = i+1;
	}
	for(int i=h;i>0;i--){
		for(int j=0;j<Chain_num-i;j++){
			if(dt2_compare[j]>dt2_compare[j+i]){
				int buffer = dt2_compare[j];
				dt2_compare[j] = dt2_compare[j+i];
				dt2_compare[j+i] = buffer;
				int buffer_id = Chain_id[j];
				Chain_id[j] = Chain_id[j+i];
				Chain_id[j+i] = buffer_id;
			}
		}
	}

	c1->cd();
	c1->Clear();
	c1->Divide(1,1);

	text->SetTextColor(kRed);
	text->SetTextSize(0.2);

	c1->cd(1);
	text->DrawTextNDC(0.35,0.8,"Tasks");

	text->SetTextSize(0.13);

	int d = 0;
	for(int i=0;i<Chain_num;i++){
		if(dt2_compare[i]>0){
			c1->cd(1);
			// c1->SetCanvasSize(1,1);
			// text->DrawTextNDC(0.1,0.75-d*0.1, Form("Chain %d",Chain_id[i]));
			int hc=dt2_compare[i]/3600;
			int mc=(dt2_compare[i]%3600)/60;
			int sc=dt2_compare[i]%60;
			text->DrawTextNDC(0,0.6-d*0.25, Form("%d.    Chain %d   %02d:%02d:%02d  "+process_current[Chain_id[i]-1]+" -> "+process_ev[Chain_id[i]-1],d+1,Chain_id[i],hc,mc,sc));

			d++;
		}
		if(d==5){break;}
	}
	c1->Modified();
	c1->Update();
}

Task::Task(TGHorizontalFrame *fMainFrame) : TGVerticalFrame(fMainFrame,width_subframe,height_task,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","Task", this, "Compare()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
    // embedded canvas
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas("c", 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
    //size of timer
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_subframe,height_task);
	
	timer->TurnOn();
}

class Setting : public TGVerticalFrame{
	private:
	TText *text;
	TCanvas *c1;
	public:
	Setting(TGHorizontalFrame *fMainFrame);
	~Setting(){};
	ClassDef(Setting,0)
};

Setting::Setting(TGHorizontalFrame *fMainFrame) : TGVerticalFrame(fMainFrame,width_setting,height_setting,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
    // embedded canvas
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas("c", 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
    //size of timer
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_setting,height_setting);
   	// fRootEmbeddedCanvas1->MoveResize(0,0,width_subframe/2,height_setting);

	c1->cd();
	c1->Clear();
	c1->Divide(2,1);

	c1->cd(1);

	text->SetTextColor(kRed);

	text->SetTextSize(0.2);
	text->DrawTextNDC(0.1,0.9, "Process");
	c1->cd(2);
	text->DrawTextNDC(0.1,0.9, "Time(Min)");
	for(int i=0;i<k;i++){
		c1->cd(1);
		text->DrawTextNDC(0.1,0.75-i*0.1, process[i]);
		c1->cd(2);
		text->DrawTextNDC(0.1,0.75-i*0.1, Form("%d",processtime_min[i]));
	}

}

class Clock : public TGHorizontalFrame{
	private:
	TTimer *timer;
	time_t t;
	TDatime CT;
	TCanvas *c1;
	TText *text;
	public:
	Clock(TGVerticalFrame *fMainFrame);
	~Clock(){if(timer) timer->TurnOff();};
	void OnTimer();
	ClassDef(Clock,0) // Development 
};

void Clock::OnTimer(){
	time(&t);

	c1->cd();
	c1->Clear();

	// Time(hours:minutes)
	text->SetTextSize(0.8);
	text->SetTextColor(kRed);
	CT.Set(t);
	text->DrawTextNDC(0.1,0.4, Form("%02d:%02d", CT.GetHour(),CT.GetMinute()));

	// Time(seconds)
	text->SetTextSize(0.5);
	text->DrawTextNDC(0.75,0.4, Form("%02d",CT.GetSecond()));

	// Date
	text->SetTextSize(0.3);
	text->DrawTextNDC(0.05,0.15, Form("%02d",CT.GetDay()));
	text->DrawTextNDC(0.25,0.15, Form("%02d",CT.GetMonth()));
	text->DrawTextNDC(0.45,0.15, Form("%04d",CT.GetYear()));
	text->DrawTextNDC(0.75,0.075, day_of_week[CT.GetDayOfWeek()-1]);

	text->SetTextSize(0.15);
	text->DrawTextNDC(0.06,0, "Day");
	text->DrawTextNDC(0.24,0, "Month");
	text->DrawTextNDC(0.5,0, "Year");
	// text->DrawTextNDC(0.6,0, day_of_week[CT.GetDayOfWeek()-1]);

	c1->Modified();
	c1->Update();
	
}

Clock::Clock(TGVerticalFrame *fMainFrame) : TGHorizontalFrame(fMainFrame,width_subframe,height_clock,kRaisedFrame){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","Clock", this, "OnTimer()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   
    // embedded canvas
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c"), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    //size of timer
   	fRootEmbeddedCanvas1->MoveResize(0,0,width_subframe,height_clock);
	
	timer->TurnOn();
}

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
			TString cmd = "say -v "+who_say+" \""+fMessage+"\""+" &"; 
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
	FILE *fp,*fl;
	bool skip_if = false;
	bool reset_if = false;
	int dt;
	int sh, sm, ss, start;
	time_t t0, t, t1, t2, total;
	TDatime T0,CT;
	int ID;
	TCanvas *c1,*c2;
	TText *text;
	TObjArray *fMsgEvents;
	TObjArray *fTimeEvents,*fPreTimeEvents;
	TGTextButton *fb1 = new TGTextButton(this,"Start");
	// TGTextButton *fb3 = new TGTextButton(this,"Resume");
	// TGTextButton *fb2 = new TGTextButton(this,"Stop");
	TGTextButton *fb4 = new TGTextButton(this,"Skip");
	TGTextButton *fb5 = new TGTextButton(this,"Unlock");
	TGTextButton *fb6 = new TGTextButton(this,"Reset");
	TGTextButton *fb7 = new TGTextButton(this,"Lock");
	TGTextButton *fb8 = new TGTextButton(this,"Set0");
	public:
	DevTimer(TGVerticalFrame *fMainFrame, int id);
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

void DevTimer::SpeechTime(time_t t){
	char buf[256];
	sprintf(buf, "say -v "+who_say+" %02d:%02d:%02d", T0.GetHour(), T0.GetMinute(), T0.GetSecond());
	gSystem->Exec(buf);
}

void DevTimer::Start(){
   	time(&t0);
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

   	// SpeechTime(t0);
	if(!skip_if){
		TString id = Form("%d",ID);
		TString Date  =	Form("%d",T0.GetDate());
		TString Hour  =	Form("%02d",T0.GetHour());
		TString Minute=	Form("%02d",T0.GetMinute());
		TString Second=	Form("%02d",T0.GetSecond());
		remove("log/skiplog/Chain"+id+".log");
		fp = fopen("log/skiplog/Chain"+id+".log", "at");
   		fl = fopen("log/Chain"+id+"/"+Date+":"+Hour+":"+Minute+":"+Second+".log", "at");
		fprintf(fp, "%ld %02d %02d %02d\n", t0, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
		fprintf(fl, "Start Developing at %02d:%02d:%02d\n", T0.GetHour(), T0.GetMinute(), T0.GetSecond());
		fclose(fp);
	}
	
   	fprintf(stdout, "Chain %2d, Start at %02d:%02d:%02d\n", ID, T0.GetHour(), T0.GetMinute(), T0.GetSecond());
   	
	skip_if = false;
	reset_if = false;

	fb1->SetState(kButtonDisabled);
	fb4->SetState(kButtonDisabled);
	fb8->SetState(kButtonDisabled);

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
	fp = fopen("log/skiplog/Chain"+id+".log","r");
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
	int h=dt/3600;
	int m=(dt%3600)/60;
	int s=dt%60;
	c1->cd();
	c1->Clear();
	c1->Divide(2,1);
	c1->cd(1);
	text->SetTextSize(1.0);
	text->SetTextColor(kRed);
	if(reset_if){text->DrawTextNDC(0.02,0.1, " "); dt2[ID-1]=0;}
	else{
		text->DrawTextNDC(0.02,0.1, Form("%02d:%02d:%02d", h,m,s));
		text->SetTextSize(0.5);
		c1->cd(2);
		Event *ev_current = NULL;
		Event *ev_next = NULL;
		// int dt2;
		for(int i=0;i<fTimeEvents->GetEntries();i++){
			Event *ev = (Event *)fTimeEvents->At(i);
			dt2[ID-1] = ev->RemainingTime(dt);
			if(dt2[ID-1]>0){
				int h2=dt2[ID-1]/3600;
				int m2=(dt2[ID-1]%3600)/60;
				int s2=dt2[ID-1]%60;
				process_ev[ID-1]=ev->GetMessage();
				text->DrawTextNDC(0.02,0.1,Form("%02d:%02d:%02d to %s", h2,m2,s2,ev->GetMessage()));
				ev_next = ev;

				int dt3 = -ev_current->RemainingTime(dt);
				int h3=dt3/3600;
				int m3=(dt3%3600)/60;
				int s3=dt3%60;
				process_current[ID-1]=ev_current->GetMessage();
				text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d %s", h3,m3,s3, ev_current->GetMessage()));

				break;
			}
			ev_current = ev;
		}
		if(NULL==ev_next){
			text->DrawTextNDC(0.02,0.1,"     since finished");

			int dt3 = -ev_current->RemainingTime(dt);
			int h3=dt3/3600;
			int m3=(dt3%3600)/60;
			int s3=dt3%60;
			text->DrawTextNDC(0.02,0.55,Form("%02d:%02d:%02d passed", h3,m3,s3));
		}
	}

	c1->Modified();
	c1->Update();
	
	for(int i=0;i<fMsgEvents->GetEntries();i++){
		Event *ev = (Event *)fMsgEvents->At(i);
		ev->Check(dt);
	}
}

DevTimer::DevTimer(TGVerticalFrame *fMainFrame, int id) : TGHorizontalFrame(fMainFrame,width_mainframe,HorizontalSize/Chain_num,kRaisedFrame), ID(id){

	SetBackgroundColor(TColor::Number2Pixel(kBlack));
	fTimeEvents = new TObjArray;
	for(int i=0;i <= k;i++){
		fTimeEvents->Add( new Event(processmin[i],0,process[i]));
	}
	
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
	TString start   = "start";

	fMsgEvents->Add( new Event(processmin[0],   0, start+" "+process[0]+" "+chain));

	for(int i=1;i<=k;i++){
		fMsgEvents->Add( new Event(processmin[i], -60, chain+" "+process[i]+" "+in1min));
		fMsgEvents->Add( new Event(processmin[i], -30, chain+" "+process[i]+" "+in30sec));
		fMsgEvents->Add( new Event(processmin[i], -10, in10sec));
		fMsgEvents->Add( new Event(processmin[i],  -5, in5sec));
		fMsgEvents->Add( new Event(processmin[i],  -4, in4sec));
		fMsgEvents->Add( new Event(processmin[i],  -3, in3sec));
		fMsgEvents->Add( new Event(processmin[i],  -2, in2sec));
		fMsgEvents->Add( new Event(processmin[i],  -1, in1sec));
		if(i!=k){
			fMsgEvents->Add( new Event(processmin[i],   0, start+" "+process[i]+" "+chain));
		}
		else{
			fMsgEvents->Add( new Event(processmin[i],   0, chain+process[i]));
		}
	}
   
   	timer = new TTimer(100);
   	timer->Connect("Timeout()","DevTimer", this, "OnTimer()");
	
	text = new TText();
	text->SetTextSize(0.5);

   	SetLayoutBroken(kTRUE);
   	fLabelChain = new TGLabel(this,Form("Chain %d", id));
   	fLabelChain->SetTextJustify(36);
   	fLabelChain->SetMargins(0,0,0,0);
   	fLabelChain->SetWrapLength(-1);
   	fLabelChain->Resize(100,80);
   	fLabelChain->SetBackgroundColor(TColor::Number2Pixel(kBlack));
   	fLabelChain->SetTextColor(TColor::Number2Pixel(kRed+1));

   	AddFrame(fLabelChain, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fLabelChain->MoveResize(3,3,100,20);

   	fLabel2 = new TGLabel(this," Start at - - : - - : - -  ");
   	fLabel2->SetBackgroundColor(TColor::Number2Pixel(kBlack));
	fLabel2->SetTextColor(TColor::Number2Pixel(kRed+1));

   	fLabel2->SetTextJustify(36);
   	fLabel2->SetMargins(0,0,0,0);
   	fLabel2->SetWrapLength(-1);
   	AddFrame(fLabel2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fLabel2->MoveResize(3,25,120,20);

   	//Button
   	fb1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb1->Connect("Clicked()", "DevTimer", this, "Start()");
   	AddFrame(fb1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb1->MoveResize(rightbutton,50,button_width,button_height);

   	// fb2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	// fb2->Connect("Clicked()", "DevTimer", this, "Stop()");
   	// AddFrame(fb2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	// fb2->MoveResize(10,80,50,20);

   	// fb3->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	// fb3->Connect("Clicked()", "DevTimer", this, "Resume()");
   	// AddFrame(fb3, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	// fb3->MoveResize(65,80,50,20);

   	fb4->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb4->Connect("Clicked()", "DevTimer", this, "Skip()");
	AddFrame(fb4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb4->MoveResize(centerbutton,50,button_width,button_height);

	fb5->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb5->Connect("Clicked()", "DevTimer", this, "Unlock()");
   	AddFrame(fb5, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb5->MoveResize(centerbutton,75,button_width,button_height);
	fb5->SetState(kButtonDisabled);

	fb6->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb6->Connect("Clicked()", "DevTimer", this, "Reset()");
   	AddFrame(fb6, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb6->MoveResize(leftbutton,75,button_width,button_height);
	fb6->SetState(kButtonDisabled);

	fb7->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb7->Connect("Clicked()", "DevTimer", this, "Lock()");
   	AddFrame(fb7, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb7->MoveResize(rightbutton,75,button_width,button_height);
	fb7->SetState(kButtonDisabled);

	fb8->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
   	fb8->Connect("Clicked()", "DevTimer", this, "Set0()");
   	AddFrame(fb8, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fb8->MoveResize(leftbutton,50,button_width,button_height);
	fb8->SetState(kButtonDisabled);

   
    // embedded canvas
   	TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,this,386,102);
   	Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   	c1 = new TCanvas(Form("c%d_1",ID), 10, 10, wfRootEmbeddedCanvas1);
   	fRootEmbeddedCanvas1->AdoptCanvas(c1);
   	AddFrame(fRootEmbeddedCanvas1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    //size of timer
   	fRootEmbeddedCanvas1->MoveResize(timer_right,0,800,95);

}

class DevTimers : public TGMainFrame {
	private:
	TObjArray eTimers;
	
	public:
	DevTimers();
	~DevTimers(){for(int i=0; i<eTimers.GetEntries(); i++){delete eTimers.At(i);}}
	
	ClassDef(DevTimers,0) // Development 
};

DevTimers::DevTimers() : TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kHorizontalFrame){
	
	SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	TGVerticalFrame *frame1= new TGVerticalFrame((TGMainFrame *)this,0,0,kSunkenFrame);
	AddFrame(frame1,new TGLayoutHints(kLHintsExpandY | kLHintsLeft,2,0,2,2));
	frame1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	for(int i=1; i<=Chain_num; i++){
		DevTimer *t = new DevTimer((TGVerticalFrame *)frame1, i);
		frame1->AddFrame( t,  new TGLayoutHints(kLHintsTop,2,2,2,2));
		eTimers.Add(t);
	}






	TGVerticalFrame *frame2= new TGVerticalFrame((TGMainFrame *)this,0,0,kSunkenFrame|kVerticalFrame);
	AddFrame(frame2,new TGLayoutHints(kLHintsExpandY | kLHintsLeft,0,2,2,2));
	frame2->SetBackgroundColor(TColor::Number2Pixel(kRed+1));

	Clock *c = new Clock((TGVerticalFrame *)frame2);
	frame2->AddFrame(c,new TGLayoutHints(kLHintsExpandX|kLHintsTop,2,2,2,2));


	// Task *task = new Task((TGHorizontalFrame *)frame2);
	// frame2->AddFrame(task,new TGLayoutHints(kLHintsExpandX|kLHintsTop,2,2,2,2));


	// TGHorizontalFrame *frame2_1 = new TGHorizontalFrame((TGVerticalFrame *)frame2,0,0,kSunkenFrame|kVerticalFrame);
	// frame2->AddFrame(frame2_1,new TGLayoutHints(kLHintsExpandX|kLHintsTop,0,0,0,0));
	// frame2_1->SetBackgroundColor(TColor::Number2Pixel(kRed+1));
	

	// Setting *s = new Setting((TGHorizontalFrame *)frame2_1);
	// frame2_1->AddFrame(s,new TGLayoutHints(kLHintsExpandY|kLHintsLeft,2,2,2,2));
	

	SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
	MapSubwindows();

	Resize(GetDefaultSize());
	MapWindow();
	Resize(500,860);
}










void developtimer()
{
	// for(int i=10;i>0;i--){
	// 	printf("%d",i);
	// }
	k = 0;
	fs = fopen("setting.txt","r");
	fscanf(fs,"The number of chains:%d",&Chain_num);
	printf("The number of chains:%d\n",Chain_num);
	printf("-----------------------\n");
	printf("| Time  | Process\n");
	printf("-----------------------\n");
	while(fscanf(fs,"%d %s",&processtime_min[k],process[k]) != EOF){
		if(k==0){
			processtime[k] = processtime_min[k]*60;
			processmin[k] = 0;
		}
		else{
			processtime[k] = processtime_min[k]*60+processtime[k-1];
			processmin[k] = processmin[k-1]+processtime_min[k-1];
		}
		printf("| %dmin | %s\n",processtime_min[k],process[k]);
		k++;
	}
	printf("-----------------------\n");
	processmin[k] = processmin[k-1]+processtime_min[k-1];
	process[k][0] = 'f'; process[k][1] = 'i'; process[k][2] = 'n'; process[k][3] = 'i'; process[k][4] = 's'; process[k][5] = 'h';
	fclose(fs);

	gStyle->SetCanvasColor(kBlack);
	new DevTimers;
}  







//stopはアバウトで1.5まで