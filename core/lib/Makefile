# -------- environment variables
FORTISSIMO_REPO = fortissimo-dir
HOST=x86_64-linux-gnu
# For Windows, use:
#HOST=x86_64-w64-mingw32

INTERNAL=false

# -------- standard targets

all : fortissimo

download-only: $(FORTISSIMO_REPO)
	cd $(FORTISSIMO_REPO) && make HOST=$(HOST) dependencies-download-only

mrclean :
	rm -rf $(FORTISSIMO_REPO)
	rm -f $(wildcard *.success)

# --------- Utility functions
define gitclone_and_checkout
	# ---- Target directory is 1st parameter
	rm -rf $(1).tmp
	rm -rf $(1)

	# ---- Git(hub|lab) user 2nd parameter, project is 3rd parameter.
	# ---- Git hash is 3rd parameter
	if [ -n "$(CI_JOB_TOKEN)" ] ; then \
		git clone "https://gitlab-ci-token:$(CI_JOB_TOKEN)@gitlab.stealthsoftwareinc.com/$(2)/$(3)" $(1).tmp ; \
	elif [ '$(INTERNAL)' =  'true' ] ; then \
		git clone "git@gitlab.stealthsoftwareinc.com:$(2)/$(3)" $(1).tmp ; \
		cd $(1).tmp && git checkout $(4) ; \
	else \
		git clone "https://github.com/stealthsoftwareinc/$(3)" $(1).tmp ; \
		cd $(1).tmp && git checkout $(5) ; \
	fi

	mv $(1).tmp $(1)

endef

# ---------- Fortissimo Definitions
$(FORTISSIMO_REPO) :
	$(call gitclone_and_checkout,$(FORTISSIMO_REPO),stealth,fortissimo.git,7849b3973d1296323084776e0ddae56b8814f027,8692c09e6e663058dd50677d12bfa29f9f3a6c53)

fortissimo.success: $(FORTISSIMO_REPO)
	cd $(FORTISSIMO_REPO) && \
		make INTERNAL=$(INTERNAL) HOST=$(HOST) dependencies \
	;
	echo 'success' > fortissimo.success

fortissimo: fortissimo.success
