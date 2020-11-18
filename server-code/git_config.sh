git config --global push.default simple

git config --global pull.rebase true  

git config --global alias.st status


git config --global diff.tool bc4

git config --global difftool.bc4.cmd 'C:\Program Files\Beyond Compare 4\BCompare.exe $LOCAL $REMOTE'

git config --global merge.tool bc4

git config --global mergetool.bc4.cmd 'C:\Program Files\Beyond Compare 4\BCompare.exe $LOCAL $REMOTE $BASE $MERGED'

git config --global mergetool.bc4.trustExitCode 

git config --global mergetool.keepBackup false