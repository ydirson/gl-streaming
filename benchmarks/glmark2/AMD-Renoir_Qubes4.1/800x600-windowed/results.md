| test | dom0 | soft | tcp-socat-qrexec-socat-tcp | tcp-socat-qrexec-stdio |
| :--- | :-: | :-: | :-: | :-: |
| [build] use-vbo=false | 10297 | 403 (/25.5509 -96%) | 28 (/367.75 -99%) | 782 (/13.1675 -92%) | 
| [build] use-vbo=true | 12788 | 402 (/31.8109 -96%) | 238 (/53.7311 -98%) | 868 (/14.7327 -93%) | 
| [texture] texture-filter=nearest | 10867 | 438 (/24.8105 -95%) | 111 (/97.9009 -98%) | 853 (/12.7397 -92%) | 
| [texture] texture-filter=linear | 10866 | 429 (/25.3287 -96%) | 116 (/93.6724 -98%) | 853 (/12.7386 -92%) | 
| [texture] texture-filter=mipmap | 10420 | 417 (/24.988 -95%) | 113 (/92.2124 -98%) | 854 (/12.2014 -91%) | 
| [shading] shading=gouraud | 10039 | 345 (/29.0986 -96%) | 238 (/42.1807 -97%) | 867 (/11.579 -91%) | 
| [shading] shading=blinn-phong-inf | 9629 | 333 (/28.9159 -96%) | 220 (/43.7682 -97%) | 868 (/11.0933 -90%) | 
| [shading] shading=phong | 9500 | 310 (/30.6452 -96%) | 194 (/48.9691 -97%) | 865 (/10.9827 -90%) | 
| [shading] shading=cel | 9158 | 315 (/29.073 -96%) | 220 (/41.6273 -97%) | 865 (/10.5873 -90%) | 
| [bump] bump-render=high-poly | 5738 | 199 (/28.8342 -96%) | 166 (/34.5663 -97%) | 863 (/6.6489 -84%) | 
| [bump] bump-render=normals | 13007 | 435 (/29.9011 -96%) | 170 (/76.5118 -98%) | 863 (/15.0718 -93%) | 
| [bump] bump-render=height | 12694 | 428 (/29.6589 -96%) | 96 (/132.229 -99%) | 844 (/15.0403 -93%) | 
| [effect2d] kernel=0,1,0;1,-4,1;0,1,0; | 7868 | 374 (/21.0374 -95%) | 574 (/13.7073 -92%) | 886 (/8.88036 -88%) | 
| [effect2d] kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1; | 3876 | 320 (/12.1125 -91%) | 558 (/6.94624 -85%) | 887 (/4.36979 -77%) | 
| [pulsar] light=false:quads=5:texture=false | 9410 | 423 (/22.2459 -95%) | 65 (/144.769 -99%) | 770 (/12.2208 -91%) | 
| [desktop] blur-radius=5:effect=blur:passes=1:separable=true:windows=4 | 3688 | 138 (/26.7246 -96%) | 61 (/60.459 -98%) | 469 (/7.86354 -87%) | 
| [desktop] effect=shadow:windows=4 | 7729 | 265 (/29.166 -96%) | 55 (/140.527 -99%) | 307 (/25.1759 -96%) | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=map | 1528 | 188 (/8.12766 -87%) | - | - | 
| [buffer] columns=200:interleave=false:update-dispersion=0.9:update-fraction=0.5:update-method=subdata | 2072 | 188 (/11.0213 -90%) | 41 (/50.5366 -98%) | 296 (/7 -85%) | 
| [buffer] columns=200:interleave=true:update-dispersion=0.9:update-fraction=0.5:update-method=map | 2075 | 236 (/8.79237 -88%) | - | - | 
| [ideas] speed=duration | 6048 | 338 (/17.8935 -94%) | 53 (/114.113 -99%) | 281 (/21.5231 -95%) | 
| [jellyfish] &lt;default&gt; | 6302 | 232 (/27.1638 -96%) | 67 (/94.0597 -98%) | 778 (/8.10026 -87%) | 
| [terrain] &lt;default&gt; | 505 | 22 (/22.9545 -95%) | 61 (/8.27869 -87%) | 481 (/1.0499 -4%) | 
| [shadow] &lt;default&gt; | 6845 | 207 (/33.0676 -96%) | 67 (/102.164 -99%) | 785 (/8.71975 -88%) | 
| [refract] &lt;default&gt; | 850 | 40 (/21.25 -95%) | 66 (/12.8788 -92%) | 756 (/1.12434 -11%) | 
| [conditionals] fragment-steps=0:vertex-steps=0 | 10065 | 390 (/25.8077 -96%) | 456 (/22.0724 -95%) | 875 (/11.5029 -91%) | 
| [conditionals] fragment-steps=5:vertex-steps=0 | 9857 | 383 (/25.7363 -96%) | 473 (/20.8393 -95%) | 874 (/11.278 -91%) | 
| [conditionals] fragment-steps=0:vertex-steps=5 | 9876 | 389 (/25.3882 -96%) | 466 (/21.1931 -95%) | 875 (/11.2869 -91%) | 
| [function] fragment-complexity=low:fragment-steps=5 | 9891 | 384 (/25.7578 -96%) | 467 (/21.1799 -95%) | 875 (/11.304 -91%) | 
| [function] fragment-complexity=medium:fragment-steps=5 | 9672 | 372 (/26 -96%) | 477 (/20.2767 -95%) | 875 (/11.0537 -90%) | 
| [loop] fragment-loop=false:fragment-steps=5:vertex-steps=5 | 9441 | 382 (/24.7147 -95%) | 423 (/22.3191 -95%) | 874 (/10.8021 -90%) | 
| [loop] fragment-steps=5:fragment-uniform=false:vertex-steps=5 | 9889 | 381 (/25.9554 -96%) | 425 (/23.2682 -95%) | 875 (/11.3017 -91%) | 
| [loop] fragment-steps=5:fragment-uniform=true:vertex-steps=5 | 9897 | 377 (/26.252 -96%) | 410 (/24.139 -95%) | 874 (/11.3238 -91%) | 
| glmark2 Score | 7951 | 317 (/25.082 -96%) | 231 (/34.4199 -97%) | 772 (/10.2992 -90%) | 
