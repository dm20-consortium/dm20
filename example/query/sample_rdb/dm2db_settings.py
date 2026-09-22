#!/usr/bin/python3
import os
import sys
import psycopg2

conn_parameter="host=127.0.0.1 port=5432 user=dm2 password=dm2rdbsamplepassword"
first_set = 'SET search_path = dm2;'

def usage():
  print("[使用方法1] コマンド入力")
  print("  python3 dm2db_settings.py dm2_db 'select * from table'")
  print("[使用方法2] ファイル入力")
  print("  python3 dm2db_settings.py dm2_db insert.sql")
  print("[使用方法3] COPY文の実行は、copyの後に、テーブル名とファイル入力")
  print("  python3 dm2db_settings.py dm2_db copy table dump_file")

def getFileValue(path):
  file_value = ""
  dump_file_value = ""
  if os.path.isfile(path):
    # ファイル読込み
    try:
      f = open(path, 'r')
    except IOError:
      print('%s cannot be opened.' % path)
      exit()
    file_value = f.read()
    f.close()
  return file_value

if __name__=="__main__":
  args = sys.argv
  if len(args) <= 2:
      usage()
      exit()
  db_name = args[1]
  conn_parameter = conn_parameter + " dbname=" + db_name
  path = args[2]
  copy_table_name = ""
  dump_file_name = ""

  file_value = getFileValue(path)
  if file_value == "":
    # ファイルが存在しないなら、入力内容をセット。
    query = path
  else:
    # ファイルが存在すればファイルの中身をセット。
    query = file_value
  
  set_schema = False
  if 'dm2_db' in db_name:
    set_schema = True
  mode = 'select'
  if 'select' in query.lower():
    mode = 'select'
  elif 'copy' in query.lower():
    mode = 'copy'
    if len(args) <= 4:
      usage()
      exit()
    copy_table_name = args[3]
    dump_file_name = args[4]
    
  else:
    mode = 'other'
    if 'create' in query.lower():
        if 'schema' in query.lower():
            set_schema = False
  return_value = False
  # DB接続
  conn = psycopg2.connect(conn_parameter)
  try:
      with conn.cursor() as cur:
        if set_schema:
          print(first_set)
          cur.execute(first_set)
        if mode == 'select':
          print(query)
          cur.execute(query)
          if cur.fetchone() is not None:
            return_value = True
        elif mode == 'copy':
          print("copy " + copy_table_name + " from " + dump_file_name)
          f = open(dump_file_name, 'r', encoding='utf-8')
          cur.copy_from( f , copy_table_name, sep='\t', null='\\N')
          f.close()
        else:
          print(query)
          cur.execute(query)
        conn.commit()
  finally:
      conn.close()
  sys.exit(return_value)
