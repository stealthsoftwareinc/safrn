#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# gitlab_ci_docker_push_here <saved_image> <suffix> <config_h>
#

if [[ "$(type -t \
gitlab_ci_docker_push_here)" != function ]]; then
gitlab_ci_docker_push_here() {

  case $# in
    3)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  local saved_image
  saved_image=$1
  readonly saved_image

  local suffix
  suffix=$2
  readonly suffix

  local config_h
  config_h=$3
  readonly config_h

  local tag
  tag=$(
    config_h_get_string \
      $config_h \
      PACKAGE_VERSION_DOCKER \
    ;
  )
  readonly tag

  local src
  src=$(docker load -q <$saved_image | sed -n '1s/.*: //p')
  readonly src

  local dst
  dst=$CI_REGISTRY_IMAGE/$CI_COMMIT_REF_SLUG$suffix
  readonly dst

  docker tag $src $dst:$tag
  docker tag $src $dst:latest

  docker login -u $CI_REGISTRY_USER --password-stdin $CI_REGISTRY <<EOF
$CI_REGISTRY_PASSWORD
EOF

  docker push $dst:$tag
  docker push $dst:latest

}; readonly -f gitlab_ci_docker_push_here; fi
