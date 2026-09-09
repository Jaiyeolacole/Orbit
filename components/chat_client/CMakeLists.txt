# NOTE: the component manager fetches espressif/openai into
# managed_components/. It is normally registered under the plain name
# "openai" (the namespace prefix is only used for the on-disk folder name),
# so that's what we REQUIRE here. If a build fails to resolve this
# component, check the folder name under managed_components/ after the
# first `idf.py reconfigure` and update REQUIRES to match.
idf_component_register(
    SRCS "chat_client.c"
    INCLUDE_DIRS "include"
    REQUIRES openai
)
