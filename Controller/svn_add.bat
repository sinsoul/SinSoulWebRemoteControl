@echo off
svn add ./*
svn add ./assets/*
svn add ./assets/css/*
svn add ./assets/img/*
svn add ./assets/img/ui/*
svn add ./assets/img/icons/*
svn add ./assets/js/*
svn commit -m "new version submit"
echo ÈÎÒâ¼üÍË³ö...
pause >nul