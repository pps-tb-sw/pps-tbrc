import sqlite3
import os
from datetime import datetime

def main():
  path = os.getenv('PPS_PATH', '/home/ppstb/pps-tbrc/build/')
  conn = sqlite3.connect(path+'/run_infos.db')
  c = conn.cursor()
  c.execute('SELECT id,start FROM run ORDER BY id DESC LIMIT 1')
  run_id, start_ts = c.fetchone()
  print 'Last Run number is:', run_id
  print '  -- started on', datetime.fromtimestamp(int(start_ts))
  c.execute('SELECT COUNT(*) FROM burst WHERE run_id=%i' % (run_id))
  print '  -- %i burst(s) recorded so far' % c.fetchone()[0]
  c.execute('SELECT burst_id,start FROM burst WHERE run_id=%i ORDER BY id DESC LIMIT 1' % (run_id))
  last_burst_id, last_burst_start_ts = c.fetchone()
  print '  -- last burst: #%i started on %s' % (last_burst_id, datetime.fromtimestamp(int(last_burst_start_ts)))
  print '  -- currently attached:'
  c.execute('SELECT tdc_id,tdc_address,tdc_det_mode,tdc_acq_mode,detector FROM tdc_conditions WHERE run_id=%i'%(run_id))
  for l in c.fetchall():
    if int(l[2])==0: det_mode = 'pair'
    elif int(l[2])==1: det_mode = 'trailing only'
    elif int(l[2])==2: det_mode = 'leading only'
    elif int(l[2])==3: det_mode = 'trailing/leading'
    if int(l[3])==0: acq_mode = 'continuous storage'
    elif int(l[3])==1: acq_mode = 'trigger matching'
    print '      (board%i) %10s on board 0x%08x -- %s mode (%s)' % (l[0], l[4], l[1], acq_mode, det_mode)

if __name__=='__main__':
  main()
