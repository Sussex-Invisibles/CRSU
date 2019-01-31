// function to stretch axis of graph
void rescaleaxis(TGraph *g, double scale, double shift) {
  int N=g->GetN();
  double *x=g->GetX();
  int i=0;
  while(i<N) {
    x[i]=(x[i]-shift)*scale;
    i=i+1;
  }
  g->GetHistogram()->Delete();
  g->SetHistogram(0);
};

// main function
void ping_new() {

  const float MINPING = 5e-2;
  const float MAXPING = 9e3;

  // input path/file
  string fpath = "1B4_longrun/";
  string inputs[] = {"sussex","oxford","buffer1","dbug","vpn"};
  
  // start time (offset)
  TDatime tstart(2018,05,24,18,00,00);
  int T0 = tstart.Convert();
  
  ifstream in[5];
  TGraph g[5], h[5];
  double tstamp, val, maxval;
  double tmin=2e9, tmax=0;
  
  for (int i=0; i<5; i++) {
    
    // initialise
    in[i].open((fpath+inputs[i]+"_latency.log").c_str());
    
    // fill graph
    maxval=0;
    int count=0, fails=0;
    // header
    string line;
    getline(in[i],line);
    getline(in[i],line);
    while (!in[i].eof()) {
      in[i] >> tstamp >> val;
      if (val>maxval) maxval=val;
      if (tmin>tstamp) tmin=tstamp;
      if (tstamp>tmax) tmax=tstamp;
      if (val<0) {
        val = MAXPING;
        h[i].SetPoint(fails,tstamp,1e3);
        fails++;
      } else {
        if (val>MAXPING) val=MAXPING;
        g[i].SetPoint(count,tstamp,val);
        count++;
      }
    }
    in[i].close();
    
  }
  
  double elapsed = tmax-tmin;
  printf("Elapsed time is %lf seconds.\n",elapsed);
  double scale = 1.;
  string tu = "s"; // time unit
  if (elapsed > 3*60) { // convert to minutes
    scale = 60;
    tu = "min";
  } 
  if (elapsed > 3*3600) { // convert to hours
    scale = 3600;
    tu = "h";
  } 
  if (elapsed > 3*24*3600) { // convert to days
    scale = 24*3600;
    tu = "d";
  }
  for (int i=0; i<5; i++) {
    rescaleaxis(&g[i], 1./scale, tmin);
    rescaleaxis(&h[i], 1./scale, tmin);
  }
  
  // plot graph
  TCanvas c;
  string title = Form("Ping statistics (Shawcross);Time [%s];Latency [ms]",tu.c_str());
  c.DrawFrame((-elapsed/50)/scale,MINPING,(elapsed+elapsed/50)/scale,MAXPING,title.c_str());
  c.SetGrid();
  c.SetLogy();
  TLegend leg(0.3,0.77,0.67,0.89);
  leg.SetNColumns(2);
  int col[] = {8,92,4,2,1};
  for (int i=0; i<5; i++) {
    g[i].SetMarkerStyle(6);
    g[i].SetMarkerColorAlpha(col[i],0.5);
    g[i].SetLineColorAlpha(col[i],0.5);
    if (i==4) g[i].SetLineColorAlpha(col[i],0.5);
    g[i].SetFillColorAlpha(col[i],0.5);
    if (i==4) g[i].SetLineWidth(2);
    g[i].Draw("L same");
    h[i].SetMarkerStyle(8);
    h[i].SetMarkerColor(col[i]);
    h[i].SetLineColor(col[i]);
    //if (h.GetN()>0) h.Draw("P same");
    leg.AddEntry(&g[i],inputs[i].c_str(),"lf");
  }
  leg.AddEntry(&h[4],"vpn fails","lp");
  leg.SetLineColorAlpha(0,0);
  leg.SetFillColorAlpha(0,0);
  leg.Draw();
  g[4].Draw("L same");
  if (h[4].GetN()>0) h[4].Draw("P same");;
  // save plot
  c.Print((fpath+"latency.png").c_str());
  c.Print((fpath+"latency.pdf").c_str());
  c.Close();
  
  TCanvas d;
  d.DrawFrame((-elapsed/50)/scale,0,(elapsed+elapsed/50)/scale,500,title.c_str());
  d.SetGrid();
  //d.SetLogy();
  TLegend leg2(0.13,0.15,0.6,0.2);
  leg2.SetNColumns(3);
  //int col[] = {8,92,4,2,1};
  for (int i=2; i<5; i++) {
    g[i].Draw("P same");
    leg2.AddEntry(&g[i],inputs[i].c_str(),"lf");
  }
  //leg2.AddEntry(&h[4],"vpn fails","lp");
  leg2.SetLineColorAlpha(0,0);
  leg2.SetFillColorAlpha(0,0);
  leg2.Draw();
  g[4].Draw("P same");
  //if (h[4].GetN()>0) h[4].Draw("P same");;
  // save plot
  d.Print((fpath+"latency_zoom.png").c_str());
  d.Print((fpath+"latency_zoom.pdf").c_str());
  d.Close();
  

}

