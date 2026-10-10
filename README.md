This is a repo containing all the codes written during the course of "Informatica con Laboratorio", in the Laurea Triennale of Fisica at the University of Pisa.
I won't be writing all the codes we look at because the first ones are just used to explain basic structures, but there will be a couple of codes that I found interesting and the project of the exam.


--USEFUL INSTRUCTIONS--
To pull the teach's repo, since it's private and we're running on linux, we have to write a couple of instructions:
* cd ~/InfoConLab/RepoProf/IL2627
* eval "$(ssh-agent -s)"
* ssh-add ~/.ssh/IL2627_readonly.txt
* git pull (if it doesn't work, use next instruction: otherwise you're done)
* git clone git@github.com:unipi-informatica-lab-fisica/IL2627.git

To save different versions of your code, you can use tags: then, to compare them, use the git diff instruction. Here is a workflow of how to do this in the VSCode Linux Terminal.

-Before making a major change
git status
git add .
git commit -m "Working version before adding new block"
git tag -a v1.0 -m "Working version before adding new block"
git push origin main --tags

-After making the change
git add .
git commit -m "Add new block"
git tag -a v2.0 -m "Added new block"
git push origin main --tags

-Later, compare them
git diff --stat v1.0 v2.0
git diff v1.0 v2.0
