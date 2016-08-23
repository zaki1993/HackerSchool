from bs4 import BeautifulSoup
import requests
import re
import urllib2
import os
import cookielib
import json

def get_soup(url,header):
    return BeautifulSoup(urllib2.urlopen(urllib2.Request(url,headers=header)))


query = raw_input("Search something: ")
image_type=query
query= query.split()
query='+'.join(query)
url="https://www.google.co.in/search?q="+query+"&source=lnms&tbm=isch"
print url
DIR="/home/streetzaki/github/HackerSchool/Linux/wget/download/"
header={'User-Agent':"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.134 Safari/537.36"
}
soup = get_soup(url,header)


ActualImages=[]
for a in soup.find_all("div",{"class":"rg_meta"}):
    link , Type =json.loads(a.text)["ou"]  ,json.loads(a.text)["ity"]
    ActualImages.append((link,Type))

print  "there are total" , len(ActualImages),"images"

for i , (img , Type) in enumerate( ActualImages):
    try:
        req = urllib2.Request(img, headers={'User-Agent' : header})
        raw_img = urllib2.urlopen(req).read()
        if not os.path.exists(DIR):
            os.mkdir(DIR)
        cntr = len([i for i in os.listdir(DIR) if image_type in i]) + 1
        print cntr
        if len(Type)==0:
            f = open(DIR + image_type + "_"+ str(cntr)+".jpg", 'wb')
        else :
            f = open(DIR + image_type + "_"+ str(cntr)+"."+Type, 'wb')


        f.write(raw_img)
        f.close()
    except Exception as e:
        print "could not load : "+img
        print e