export DB_HOST=lab_04_postgres
export DB_PORT=5432
export DB_LOGIN=root
export DB_PASSWORD=root
export DB_DATABASE=archdb
export MONGO_HOST=lab04_mongo
export MONGO_PORT=27017
export MONGO_DATABASE=arch
export JWT_KEY=0123456789ABCDEF0123456789ABCDEF

mongoimport --uri 'mongodb://lab04_mongo:27017/arch' --collection posts --file import_json/PostsJson.json --jsonArray
mongoimport --uri 'mongodb://lab04_mongo:27017/arch' --collection messages --file import_json/MessagesJson.json --jsonArray

python3 -m venv ./env &&
source ./env/bin/activate &&
pip3 install -r import_json/requirements.txt &&
python3 "import_json/db_init.py"

