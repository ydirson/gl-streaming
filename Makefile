all:

%:
	$(MAKE) -C gl_server $*
	$(MAKE) -C gl_client $*
	$(MAKE) -C gl_client/samples $*
