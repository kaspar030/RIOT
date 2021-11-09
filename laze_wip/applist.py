import sys
import yaml

data = yaml.load(open(sys.argv[1]))

for app, builders in data.items():
    print(app)
    #for builder in builders:
    #    print (app, builder)
