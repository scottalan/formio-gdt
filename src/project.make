api = 2
core = 7.x

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"

; Drupal Core

includes[] = drupal-org-core.make

;; Project-specific Dependencies

includes[] = formio.make
