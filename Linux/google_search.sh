python google_search.py
mkdir download
cd download 
mkdir otherFiles 
imgCont=$(ls *.jpeg) 
counter=1; 
for i in $imgCont; do convert $i -resize 20x20! -quality 95 $i; mv -v $i google_pic_$counter.jpeg; counter=$((counter+1)); done; echo "DONE"; counter=1; 
moveFile=$(ls -p| grep -v / | grep -v .jpeg)
for i in $moveFile; do mv $i otherFiles; done
cd otherFiles
tar -zcvf archiveOfother.tar.gz *;
for i in $moveFile; do rm -v $i; done
