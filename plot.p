  # This file is called   plotData.p


  set   terminal  wxt 0 persist
  set   ylabel "weight"
  plot    "<(sed -n '2,2002p' logWeights.dat)" using 1:2 title 'W0' with linespoints

  set   terminal  wxt 1 persist
  set   autoscale                        # scale axes automatically
  unset log                              # remove any log-scaling
  unset label                            # remove any previous labels
  set xtic auto                          # set xtics automatically
  set ytic auto                          # set ytics automatically
  set title "CerebellarController"
  set xlabel "time"
  set ylabel "position"

  #set yrange [-2:2]
  plot    "<(sed -n '2,342p' logData.dat)" using 1:2 title 'Reference' with lines , \
          "<(sed -n '2,342p' logData.dat)" using 1:4 title 'Ref Model' with points, \
          "<(sed -n '2,342p' logData.dat)" using 1:3 title 'Plant' with lines#, \
 #         "<(sed -n '2,342p' logData.dat)" using 1:6 title 'Control Signal' with points
        
        
  
