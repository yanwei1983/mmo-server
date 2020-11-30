git config --global push.default simple
git config --global pull.rebase true  
git config --global alias.st status
git config --global rebase.autosquash true


git config --global diff.tool bc4
git config --global difftool.bc4.cmd 'C:\Program Files\Beyond Compare 4\BCompare.exe $LOCAL $REMOTE'
git config --global merge.tool bc4
git config --global mergetool.bc4.cmd 'C:\Program Files\Beyond Compare 4\BCompare.exe $LOCAL $REMOTE $BASE $MERGED'
git config --global mergetool.bc4.trustExitCode 
git config --global mergetool.keepBackup false


git config core.editor 'code --wait'
git config mrege.tool vscode
git config mergetool.vscode.cmd 'code --wait $MERGED'
git config diff.tool vscode
git config difftool.vscode.cmd 'code --wait --diff $LOCAL $REMOTE'


