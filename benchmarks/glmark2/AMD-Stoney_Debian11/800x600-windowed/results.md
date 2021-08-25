| test | native | gls-loopback | gls-eth-1Gb |
| :--- | :-: | :-: | :-: |
| [build] use-vbo=false | 1146 | 407 (-64%) | 125 (-89%) | 
| [build] use-vbo=true | 1439 | 564 (-60%) | 472 (-67%) | 
| [texture] texture-filter=nearest | 1299 | 570 (-56%) | 472 (-63%) | 
| [texture] texture-filter=linear | 1280 | 570 (-55%) | 471 (-63%) | 
| [texture] texture-filter=mipmap | 1334 | 571 (-57%) | 473 (-64%) | 
| [shading] shading=gouraud | 1184 | 541 (-54%) | 472 (-60%) | 
| [shading] shading=blinn-phong-inf | 1175 | 541 (-53%) | 473 (-59%) | 
| [shading] shading=phong | 1174 | 541 (-53%) | 470 (-59%) | 
| [shading] shading=cel | 1180 | 543 (-53%) | 470 (-60%) | 
| [bump] bump-render=high-poly | 1021 | 512 (-49%) | 462 (-54%) | 
| [bump] bump-render=normals | 1463 | 556 (-61%) | 468 (-68%) | 
| [bump] bump-render=height | 1470 | 569 (-61%) | 473 (-67%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 1033 | 515 (-50%) | 463 (-55%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 610 | 390 (-36%) | 377 (-38%) | 
| [pulsar] light=false:quads=5:texture=false | 1081 | 469 (-56%) | 457 (-57%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 591 | 290 (-50%) | 353 (-40%) | 
| [desktop] effect=shadow:windows=4 | 919 | 212 (-76%) | 348 (-62%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 158 | - | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 201 | 141 (-29%) | 85 (-57%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 287 | - | - | 
| [ideas] speed=duration | 989 | 124 (-87%) | 269 (-72%) | 
| [jellyfish] &lt;default&gt; | 699 | 422 (-39%) | 397 (-43%) | 
| [terrain] &lt;default&gt; | 120 | 95 (-20%) | 96 (-20%) | 
| [shadow] &lt;default&gt; | 838 | - | - | 
| [refract] &lt;default&gt; | 81 | - | - | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 1275 | 554 (-56%) | 470 (-63%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 1274 | 557 (-56%) | 475 (-62%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 1252 | 556 (-55%) | 476 (-61%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 1268 | 557 (-56%) | 475 (-62%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 1260 | 556 (-55%) | 474 (-62%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 1263 | 556 (-55%) | 474 (-62%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 1268 | 553 (-56%) | 474 (-62%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 1263 | 557 (-55%) | 472 (-62%) | 
| glmark2 Score | 996 | 468 (-53%) | 411 (-58%) | 
