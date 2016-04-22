<script type="text/javascript">
(function b($, u) {
  if (typeof $.fn.seamless === u) {
    return setTimeout(b, 100);
  }
  $(function () {
    $('#formio-form').seamless({fallback: false}).receive(function (d, e) {
    });
  });
})(jQuery);
</script>

<iframe id="formio-form" style="width:100%;border:none;" height="600px"
  src="<?php print $module_path; ?>/_view/index.html#/<?php print $project_hash; ?>/form/<?php print $_id; ?>?iframe=1<?php print $bootswatch_theme; ?>&header=0">
</iframe>
