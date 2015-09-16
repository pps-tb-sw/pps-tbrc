import sqlite3
import os

def main():
  path = os.getenv('PPS_PATH', '/home/ppstb/pps-tbrc/build/')
  conn = sqlite3.connect(path+'/run_infos.db')
  c = conn.cursor()
  c.execute('SELECT id FROM run ORDER BY id DESC LIMIT 1')
  print 'Last Run number is:', c.fetchone()[0]

if __name__=='__main__':
  main()
