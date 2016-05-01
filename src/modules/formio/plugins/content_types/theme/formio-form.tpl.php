<script type="text/javascript">
(function b($, u) {
  if (typeof $.fn.seamless === u) {
    return setTimeout(b, 100);
  }
  $(function () {
    $('#formio-form-<?php print $name . '-' . $pid; ?>').seamless({fallback: false}).receive(function (d, e) {
    });
  });
})(jQuery);
</script>

<iframe id="formio-form-<?php print $name . '-' . $pid; ?>" style="width:100%;border:none;" height="250px"
  src="/<?php print $module_path; ?>/view/index.html#/<?php print $project_hash; ?>/form/<?php print $_id; ?>?iframe=1<?php print $bootswatch_theme; ?>&header=0">
</iframe>
