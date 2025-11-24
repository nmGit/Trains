mkdir build
doxygen 

python -m breathe.apidoc -o source/generated -m -f ./build/xml
python -m sphinx.cmd.build -b html ./source/ ./build/
