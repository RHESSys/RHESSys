
from rhessystest import Settings

settings = Settings(testsites=[
                    {
                     # Name must match ${name}.zip for all test cases
                     'name': 'W8',
                     # Output prefix must be set to '-pre ../out/testcase' for all test cases
                     'cmdline': '-t ../tecfiles/tec.testcase -w ../worldfiles/world.w8.testcase  -r ../flowtables/flow.w8  -st 2003 10 1 1 -ed 2007 10 1 1 -pre ../out/testcase -s 0.812 58.038 -sv 0.812 58.038 -gw 0.042 0.716 -g -b',
                     'spatial_agg': 'basin',
                     'temporal_agg': 'daily'}
                    ])
