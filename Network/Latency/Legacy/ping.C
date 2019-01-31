// function to stretch axis of graph
void rescaleaxis(TGraph *g, double scale) {
  int N=g->GetN();
  double *x=g->GetX();
  int i=0;
  while(i<N) {
    x[i]=x[i]*scale;
    i=i+1;
  }
  g->GetHistogram()->Delete();
  g->SetHistogram(0);
};

// main function
void ping() {

  const int MAXPING = 500;

  // input file
  string input = "Shawcross/*_latency.log";
  //input = "dbug_no_load_linux";
  bool mac = false;
  if (input.find("mac")<input.length()) mac = true;

  // initialise
  ifstream in((input+".log").c_str());
  string host, line, str;
  unsigned first, last;
  getline(in,line);
  first = line.find("PING ");
  last = line.find(" (");
  host = line.substr(first+5,last-first-5);

  // fill graph
  TGraph g, h;
  int val, maxval=0, count=0, fails=0;
  while (1) {
    getline(in,line);
    if(!in.good()) break;
    if(line=="") break; // end of pinging
    if (line.find("timeout")<line.length()) {
      val = 10000;
      h.SetPoint(fails,count+1,MAXPING);
      fails++;
    } else {
      first = line.find("time=");
      last = line.find(" ms");
      str = line.substr(first+5,last-first-5);
      val = atoi(str.c_str());
      if (val>maxval) maxval=val;
    }
    if (val>MAXPING) val=MAXPING;
    g.SetPoint(count,count+1,val);
    count++;
  }
  in.close();
  
  // rescale x-axis
  float time = count*(1800./1551); // sec, calibrated using 30 min test (Linux)
  if (mac) time = count*(23*60./1378); // sec, calibrated using 23 min test (Mac)
  string tu = "s"; // time unit
  if (time>300) { time /= 60.; tu = "min"; } // convert to minutes (if >5 min)
  if (time>300) { time /= 60.; tu = "h"; }   // convert to hours (if >5 h)
  if (time>72)  { time /= 24.; tu = "d"; }   // convert to days (if >3 d)
  rescaleaxis(&g, time/count);
  rescaleaxis(&h, time/count);
 
  // plot graph 
  TCanvas c;
  string title = Form("Ping statistics (%s);Time [%s];Latency [ms]",host.c_str(),tu.c_str());
  c.DrawFrame(0,0,time,MAXPING)->SetTitle(title.c_str());
  c.SetGrid();
  g.SetMarkerStyle(7);
  g.SetMarkerColor(2);
  g.SetLineColor(2);
  g.Draw("P same");
  h.SetMarkerStyle(8);
  h.SetMarkerColor(1);
  h.SetLineColor(1);
  if (h.GetN()>0) h.Draw("P same");
  
  // add text
  if (tu=="min") {
    TLatex t;
    TLine n;
    if (mac) {
      t.DrawText(5.5,60,"VIDEO");
      t.DrawText(9.5,60,"AUDIO");
      n.DrawLine(5,0,5,MAXPING);
      n.DrawLine(9,0,9,MAXPING);
      n.DrawLine(13,0,13,MAXPING);
    } else {
      t.DrawText(6,60,"CAM"); 
      t.DrawText(13.8,60,"XSNOED"); 
      t.DrawText(20.8,60,"BOTH"); 
      n.DrawLine(5.9,0,5.9,MAXPING);
      n.DrawLine(8.9,0,8.9,MAXPING);
      n.DrawLine(11.8,0,11.8,MAXPING);
      n.DrawLine(20.5,0,20.5,MAXPING);
      n.DrawLine(24.5,0,24.5,MAXPING);
      n.DrawLine(26.6,0,26.6,MAXPING);
    }
  }

  // timeout thresholds
  TLine l(0,150,time,150);
  l.SetLineColor(4);
  l.SetLineWidth(2);
  l.Draw();
  TLine m(0,250,time,250);
  m.SetLineColor(3);
  m.SetLineWidth(2);
  m.Draw();
  TLegend leg(0.74,0.74,0.89,0.89);
  leg.AddEntry(&l,"current","l");
  //leg.AddEntry(&m," "," ");
  leg.AddEntry(&m,"proposed","l");
  leg.Draw();

  // save plot
  c.Print((input+".png").c_str());
  c.Print((input+".pdf").c_str());
  c.Close();
}

