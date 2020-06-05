coverage run -m unittest discover -s tests
coverage html
cd htmlcov
iexplore.exe index.html