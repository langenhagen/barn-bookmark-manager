
1. make class Bookmark
2. load all bookmarks from file
3. show bookmarks in view
4. vue-js the view




from: https://www.revsys.com/tidbits/recommended-django-project-layout/

In foo/foo/ make a settings directory and create an empty __init__.py file inside it.

Move `foo/foo/settings.py` into `foo/foo/settings/base.py`

Create the individual dev.py , stage.py , jenkins.py , and prod.py files in foo/foo/settings/. Each
of these 4 environment specific files should simply contain the following:
`from base import *`


open up foo/foo/settings/prod.py and after the initial import from base just add `DEBUG=False`


./manage.py migrate —settings=foo.settings.production
