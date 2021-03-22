cd system
make clean
make

mkdir /home/lcom/labs/minixop
mkdir /home/lcom/labs/minixop/data
cd ..

cp -fr data/maps /home/lcom/labs/minixop/data/maps
cp system/proj  /home/lcom/labs/minixop
cp run.sh  /home/lcom/labs/minixop/run.sh

cd
cd labs/minixop

chmod +x run.sh

echo Please go to project executable folder by executing
echo cd
echo cd labs/minixop
echo After that just use ./run.sh
