| test | native | native-tcp |
| :--- | :-: | :-: |
| [build] use-vbo=false | 1102 | 512 (/2.15234 -53%) | 
| [build] use-vbo=true | 1383 | 702 (/1.97009 -49%) | 
| [texture] texture-filter=nearest | 1189 | 672 (/1.76935 -43%) | 
| [texture] texture-filter=linear | 1203 | 673 (/1.78752 -44%) | 
| [texture] texture-filter=mipmap | 1215 | 676 (/1.79734 -44%) | 
| [shading] shading=gouraud | 1038 | 688 (/1.50872 -33%) | 
| [shading] shading=blinn-phong-inf | 1034 | 689 (/1.50073 -33%) | 
| [shading] shading=phong | 1023 | 684 (/1.49561 -33%) | 
| [shading] shading=cel | 1033 | 685 (/1.50803 -33%) | 
| [bump] bump-render=high-poly | 924 | 653 (/1.41501 -29%) | 
| [bump] bump-render=normals | 1383 | 704 (/1.96449 -49%) | 
| [bump] bump-render=height | 1374 | 679 (/2.02356 -50%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 927 | 697 (/1.32999 -24%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 534 | 494 (/1.08097 -7%) | 
| [pulsar] light=false:quads=5:texture=false | 981 | 543 (/1.80663 -44%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 531 | 310 (/1.7129 -41%) | 
| [desktop] effect=shadow:windows=4 | 853 | 196 (/4.35204 -77%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 159 | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 199 | 134 (/1.48507 -32%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 263 | - | 
| [ideas] speed=duration | 1002 | 172 (/5.82558 -82%) | 
| [jellyfish] &lt;default&gt; | 638 | 520 (/1.22692 -18%) | 
| [terrain] &lt;default&gt; | 114 | 103 (/1.1068 -9%) | 
| [shadow] &lt;default&gt; | 768 | 534 (/1.4382 -30%) | 
| [refract] &lt;default&gt; | 79 | 70 (/1.12857 -11%) | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 1141 | 681 (/1.67548 -40%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 1139 | 684 (/1.6652 -39%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 1137 | 685 (/1.65985 -39%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 1139 | 681 (/1.67254 -40%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 1133 | 681 (/1.66373 -39%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 1125 | 682 (/1.64956 -39%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 1128 | 683 (/1.65154 -39%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 1126 | 680 (/1.65588 -39%) | 
| glmark2 Score | 909 | 556 (/1.63489 -38%) | 
