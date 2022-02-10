| test | native | gls-loopback | gls-eth-1Gb |
| :--- | :-: | :-: | :-: |
| [build] use-vbo=false | 1146 | 407 (/2.81572 -64%) | 125 (/9.168 -89%) | 
| [build] use-vbo=true | 1439 | 564 (/2.55142 -60%) | 472 (/3.04873 -67%) | 
| [texture] texture-filter=nearest | 1299 | 570 (/2.27895 -56%) | 472 (/2.75212 -63%) | 
| [texture] texture-filter=linear | 1280 | 570 (/2.24561 -55%) | 471 (/2.71762 -63%) | 
| [texture] texture-filter=mipmap | 1334 | 571 (/2.33625 -57%) | 473 (/2.8203 -64%) | 
| [shading] shading=gouraud | 1184 | 541 (/2.18854 -54%) | 472 (/2.50847 -60%) | 
| [shading] shading=blinn-phong-inf | 1175 | 541 (/2.1719 -53%) | 473 (/2.48414 -59%) | 
| [shading] shading=phong | 1174 | 541 (/2.17006 -53%) | 470 (/2.49787 -59%) | 
| [shading] shading=cel | 1180 | 543 (/2.17311 -53%) | 470 (/2.51064 -60%) | 
| [bump] bump-render=high-poly | 1021 | 512 (/1.99414 -49%) | 462 (/2.20996 -54%) | 
| [bump] bump-render=normals | 1463 | 556 (/2.63129 -61%) | 468 (/3.12607 -68%) | 
| [bump] bump-render=height | 1470 | 569 (/2.58348 -61%) | 473 (/3.10782 -67%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 1033 | 515 (/2.00583 -50%) | 463 (/2.2311 -55%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 610 | 390 (/1.5641 -36%) | 377 (/1.61804 -38%) | 
| [pulsar] light=false:quads=5:texture=false | 1081 | 469 (/2.3049 -56%) | 457 (/2.36543 -57%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 591 | 290 (/2.03793 -50%) | 353 (/1.67422 -40%) | 
| [desktop] effect=shadow:windows=4 | 919 | 212 (/4.33491 -76%) | 348 (/2.6408 -62%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 158 | - | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 201 | 141 (/1.42553 -29%) | 85 (/2.36471 -57%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 287 | - | - | 
| [ideas] speed=duration | 989 | 124 (/7.97581 -87%) | 269 (/3.67658 -72%) | 
| [jellyfish] &lt;default&gt; | 699 | 422 (/1.6564 -39%) | 397 (/1.76071 -43%) | 
| [terrain] &lt;default&gt; | 120 | 95 (/1.26316 -20%) | 96 (/1.25 -20%) | 
| [shadow] &lt;default&gt; | 838 | - | - | 
| [refract] &lt;default&gt; | 81 | - | - | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 1275 | 554 (/2.30144 -56%) | 470 (/2.71277 -63%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 1274 | 557 (/2.28725 -56%) | 475 (/2.68211 -62%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 1252 | 556 (/2.2518 -55%) | 476 (/2.63025 -61%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 1268 | 557 (/2.27648 -56%) | 475 (/2.66947 -62%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 1260 | 556 (/2.26619 -55%) | 474 (/2.65823 -62%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 1263 | 556 (/2.27158 -55%) | 474 (/2.66456 -62%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 1268 | 553 (/2.29295 -56%) | 474 (/2.67511 -62%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 1263 | 557 (/2.2675 -55%) | 472 (/2.67585 -62%) | 
| glmark2 Score | 996 | 468 (/2.12821 -53%) | 411 (/2.42336 -58%) | 
