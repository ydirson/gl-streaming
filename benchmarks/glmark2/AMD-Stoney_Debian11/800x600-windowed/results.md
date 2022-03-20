| test | native | native-tcp |
| :--- | :-: | :-: |
| [build] use-vbo=false | 1095 | 553 (/1.98011 -49%) | 
| [build] use-vbo=true | 1377 | 1148 (/1.19948 -16%) | 
| [texture] texture-filter=nearest | 1215 | 1041 (/1.16715 -14%) | 
| [texture] texture-filter=linear | 1210 | 1032 (/1.17248 -14%) | 
| [texture] texture-filter=mipmap | 1232 | 1055 (/1.16777 -14%) | 
| [shading] shading=gouraud | 1068 | 951 (/1.12303 -10%) | 
| [shading] shading=blinn-phong-inf | 1059 | 947 (/1.11827 -10%) | 
| [shading] shading=phong | 1053 | 938 (/1.1226 -10%) | 
| [shading] shading=cel | 1052 | 941 (/1.11796 -10%) | 
| [bump] bump-render=high-poly | 942 | 878 (/1.07289 -6%) | 
| [bump] bump-render=normals | 1409 | 1188 (/1.18603 -15%) | 
| [bump] bump-render=height | 1409 | 1180 (/1.19407 -16%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 942 | 875 (/1.07657 -7%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 539 | 525 (/1.02667 -2%) | 
| [pulsar] light=false:quads=5:texture=false | 1009 | 817 (/1.23501 -19%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 538 | 339 (/1.58702 -36%) | 
| [desktop] effect=shadow:windows=4 | 844 | 212 (/3.98113 -74%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 160 | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 201 | 150 (/1.34 -25%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 264 | - | 
| [ideas] speed=duration | 1000 | 186 (/5.37634 -81%) | 
| [jellyfish] &lt;default&gt; | 653 | 581 (/1.12392 -11%) | 
| [terrain] &lt;default&gt; | 113 | 103 (/1.09709 -8%) | 
| [shadow] &lt;default&gt; | 774 | 688 (/1.125 -11%) | 
| [refract] &lt;default&gt; | 79 | 78 (/1.01282 -1%) | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 1151 | 1062 (/1.0838 -7%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 1146 | 1060 (/1.08113 -7%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 1129 | 1054 (/1.07116 -6%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 1119 | 1062 (/1.05367 -5%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 1118 | 1051 (/1.06375 -5%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 1148 | 1043 (/1.10067 -9%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 1143 | 1045 (/1.09378 -8%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 1146 | 1023 (/1.12023 -10%) | 
| glmark2 Score | 919 | 800 (/1.14875 -12%) | 
