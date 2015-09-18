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

if __name__=='__main__':
  main()
