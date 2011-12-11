# openFrameworks
if [ ! -f Libraries/of_preRelease_v007_linux64 ];
then
   wget -O /tmp/of_preRelease_v007_linux64.tar.gz "http://www.openframeworks.cc/versions/preRelease_v0.07/of_preRelease_v007_linux64.tar.gz"
   cd Libraries; tar xzvf /tmp/of_preRelease_v007_linux64.tar.gz; cd -
   cd of_preRelease_v007_linux64/
   cd Libraries/of_preRelease_v007_linux64/scripts/linux/ubuntu
   ./install_dependencies.sh
   apt-get install libmpg123-dev
fi


apt-get install gfortran
apt-get install python-mysqldb
apt-get install python-boto

cd Libraries/mrjob
python setup.py install
cd -
