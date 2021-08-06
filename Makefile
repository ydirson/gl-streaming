all:

%:
	$(MAKE) -C gl_server $*
	$(MAKE) -C gl_client $*

samples:
	$(MAKE) -C gl_client/samples $*
