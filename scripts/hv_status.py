import sqlite3
import os
from sys import argv
from datetime import datetime

def main(argv):
  path = os.getenv('PPS_PATH', '/home/ppstb/pps-tbrc/build/')
  conn = sqlite3.connect(path+'/run_infos.db')
  c = conn.cursor()
  if len(argv)<2:
    c.execute('SELECT id,start FROM run ORDER BY id DESC LIMIT 1')
    run_id, start_ts = c.fetchone()
    print 'Last Run number is:', run_id
  else:
    run_id = int(argv[1])
    c.execute('SELECT start FROM run WHERE id=%i' % (run_id))
    start = c.fetchone()
    if not start:
      print 'Failed to find run %i in the database!' % (run_id)
      exit(0)
    print 'Run number:', run_id
    start_ts = start[0]
  print '  -- started on', datetime.fromtimestamp(int(start_ts))
  c.execute('SELECT channel,v,i FROM hv WHERE run_id=%i'%(run_id))
  apparatus = c.fetchall()
  if len(apparatus)==0:
    print '  -- no HV conditions retrieved'
  else:
    print '  -- HV conditions:'
    for l in apparatus:
      print '      (channel %i) Vbias = %d mV / Ic = %d uA' % (l[0], l[1], l[2])

if __name__=='__main__':
  main(argv)
