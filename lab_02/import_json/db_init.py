import pandas as pd
from sqlalchemy import create_engine, MetaData, Table, Column, Integer, String, Index, DateTime
from sqlalchemy.sql import func

engine = create_engine("postgresql+psycopg2://root:root@lab_02_postgres/archdb", echo = False)

# Создаем объект MetaData
metadata = MetaData()

# Определяем таблицу с помощью класса Table
users_table = Table(
    'users', metadata,
    Column('id', Integer, primary_key=True),  # Новый столбец id с типом данных serial и первичным ключом
    Column('first_name', String),  # Столбец first_name
    Column('last_name', String),  # Столбец last_name
    Column('email', String),  # Столбец email
    Column('title', String),  # Столбец title
    Column('login', String),  # Столбец login
    Column('password', String),  # Столбец password
    extend_existing=True  # Расширяем существующую таблицу, если она уже существует
)

posts_table = Table(
    'posts', metadata,
    Column('id', Integer, primary_key=True),  # Новый столбец id с типом данных serial и первичным ключом
    Column('user_id', Integer),  # Столбец user_id
    Column('post_text', String),  # Столбец post_text
    Column('date_time', DateTime, server_default=func.now()),  # Столбец DateTime
    Column('media', String),  # Столбец media
    extend_existing=True  # Расширяем существующую таблицу, если она уже существует
)

#index = Index('fn', users_table.c.first_name)

# Создаем таблицу в базе данных (если ее нет)
metadata.create_all(engine)

#index.create(engine)

# Загружаем данные из JSON
df1 = pd.read_json(r"import_json/UsersJson.json")
df2 = pd.read_json(r"import_json/PostsJson.json")

# Добавляем столбец login
df1['login'] = df1['email']

# Сохраняем данные в таблицу 'users' в базе данных
df1.to_sql("users", con=engine, if_exists='append', index=False)
df2.to_sql("posts", con=engine, if_exists='append', index=False)
