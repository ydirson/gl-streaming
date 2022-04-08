| test | native | native-tcp-pipenotif | native-tcp-eventfdnotif |
| :--- | :-: | :-: | :-: |
| [build] use-vbo=false | 1095 | 553 (/1.98011 -49%) | 564 (/1.94149 -48%) | 
| [build] use-vbo=true | 1377 | 1148 (/1.19948 -16%) | 1175 (/1.17191 -14%) | 
| [texture] texture-filter=nearest | 1215 | 1041 (/1.16715 -14%) | 1077 (/1.12813 -11%) | 
| [texture] texture-filter=linear | 1210 | 1032 (/1.17248 -14%) | 1072 (/1.12873 -11%) | 
| [texture] texture-filter=mipmap | 1232 | 1055 (/1.16777 -14%) | 1095 (/1.12511 -11%) | 
| [shading] shading=gouraud | 1068 | 951 (/1.12303 -10%) | 980 (/1.0898 -8%) | 
| [shading] shading=blinn-phong-inf | 1059 | 947 (/1.11827 -10%) | 983 (/1.07731 -7%) | 
| [shading] shading=phong | 1053 | 938 (/1.1226 -10%) | 984 (/1.07012 -6%) | 
| [shading] shading=cel | 1052 | 941 (/1.11796 -10%) | 975 (/1.07897 -7%) | 
| [bump] bump-render=high-poly | 942 | 878 (/1.07289 -6%) | 884 (/1.06561 -6%) | 
| [bump] bump-render=normals | 1409 | 1188 (/1.18603 -15%) | 1200 (/1.17417 -14%) | 
| [bump] bump-render=height | 1409 | 1180 (/1.19407 -16%) | 1196 (/1.17809 -15%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 942 | 875 (/1.07657 -7%) | 880 (/1.07045 -6%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 539 | 525 (/1.02667 -2%) | 523 (/1.03059 -2%) | 
| [pulsar] light=false:quads=5:texture=false | 1009 | 817 (/1.23501 -19%) | 832 (/1.21274 -17%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 538 | 339 (/1.58702 -36%) | 331 (/1.62538 -38%) | 
| [desktop] effect=shadow:windows=4 | 844 | 212 (/3.98113 -74%) | 213 (/3.96244 -74%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 160 | - | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 201 | 150 (/1.34 -25%) | 153 (/1.31373 -23%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 264 | - | - | 
| [ideas] speed=duration | 1000 | 186 (/5.37634 -81%) | 188 (/5.31915 -81%) | 
| [jellyfish] &lt;default&gt; | 653 | 581 (/1.12392 -11%) | 580 (/1.12586 -11%) | 
| [terrain] &lt;default&gt; | 113 | 103 (/1.09709 -8%) | 104 (/1.08654 -7%) | 
| [shadow] &lt;default&gt; | 774 | 688 (/1.125 -11%) | 690 (/1.12174 -10%) | 
| [refract] &lt;default&gt; | 79 | 78 (/1.01282 -1%) | 76 (/1.03947 -3%) | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 1151 | 1062 (/1.0838 -7%) | 1066 (/1.07974 -7%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 1146 | 1060 (/1.08113 -7%) | 1060 (/1.08113 -7%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 1129 | 1054 (/1.07116 -6%) | 1061 (/1.06409 -6%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 1119 | 1062 (/1.05367 -5%) | 1048 (/1.06775 -6%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 1118 | 1051 (/1.06375 -5%) | 1043 (/1.07191 -6%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 1148 | 1043 (/1.10067 -9%) | 1042 (/1.10173 -9%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 1143 | 1045 (/1.09378 -8%) | 1034 (/1.10542 -9%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 1146 | 1023 (/1.12023 -10%) | 1036 (/1.10618 -9%) | 
| glmark2 Score | 919 | 800 (/1.14875 -12%) | 811 (/1.13317 -11%) | 
