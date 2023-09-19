#!/bin/sh
# Copyright 2023 Huawei Cloud Computing Technology Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


set -eu

env

readonly JUST="${PWD}/bin/tool-under-test"
readonly JUST_MR="${PWD}/bin/mr-tool-under-test"
readonly LBR="${TEST_TMPDIR}/local-build-root"
readonly OUT="${TEST_TMPDIR}/out"
readonly OUT2="${TEST_TMPDIR}/out2"

mkdir work
cd work
touch ROOT
cat > repos.json <<EOF
{ "repositories":
  { "":
    { "repository":
      { "type": "git"
      , "commit": "$COMMIT_0"
      , "pragma": {"absent": true}
      , "repository": "http://non-existent.example.org/data.git"
      , "branch": "master"
      , "subdir": "src"
      }
    , "target_root": "targets"
    }
  , "targets": {"repository": {"type": "file", "path": "targets"}}
  }
}
EOF
mkdir targets
cat > targets/TARGETS <<'EOF'
{ "":
  { "type": "generic"
  , "outs": ["out.txt"]
  , "cmds": ["head -c 1 4.txt > out.txt", "cat 2.txt >> out.txt"]
  , "deps": ["4.txt", "2.txt"]
  }
}
EOF

echo
cat repos.json
echo
CONF=$("${JUST_MR}" --norc --local-build-root "${LBR}" \
                    --remote-serve-address ${SERVE} \
                    -r ${REMOTE_EXECUTION_ADDRESS} \
                    --fetch-absent setup)
cat $CONF
echo
"${JUST}" install --local-build-root "${LBR}" -C "${CONF}" \
          -r "${REMOTE_EXECUTION_ADDRESS}" -o "${OUT}" 2>&1
grep 42 "${OUT}/out.txt"

# As the last call of just-mr had --fetch-absent, all relevent information
# about the root should now be available locally, so we can build without
# a serve or remote endpoint with still (logically) fetching absent roots.
"${JUST_MR}" --norc --just "${JUST}" --local-build-root "${LBR}" \
             --fetch-absent install -o "${OUT2}" 2>&1
grep 42 "${OUT2}/out.txt"

echo DONE