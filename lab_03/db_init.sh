export DB_HOST=lab_03_postgres
export DB_PORT=5432
export DB_LOGIN=root
export DB_PASSWORD=root
export DB_DATABASE=archdb
export MONGO_HOST=lab03_mongo
export MONGO_PORT=27017
export MONGO_DATABASE=arch

mongoimport --uri 'mongodb://lab03_mongo:27017/arch' --collection posts --file import_json/PostsJson.json --jsonArray
mongoimport --uri 'mongodb://lab03_mongo:27017/arch' --collection messages --file import_json/MessagesJson.json --jsonArray

python3 -m venv ./env &&
source ./env/bin/activate &&
pip3 install -r import_json/requirements.txt &&
python3 "import_json/db_init.py"

