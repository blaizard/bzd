# KiCad

## Create a new project

`File >> New Project...` - Go to `kicad/` and entrer a name.

- Add symbol library:

  - In Schematics Editor: `Preferences >> Manage Symbol Libraries...`
    - Nickname: `bzd`
    - Library Path: `${KIPRJMOD}/../../../kicad/lib/bzd.kicad_sym`

- Add footprint library:
  - In Footprint Editor: `Preferences >> Manage Footprint Libraries...`
    - Nickname: `bzd`
    - Library Path: `${KIPRJMOD}/../../../kicad/lib/bzd.pretty`
