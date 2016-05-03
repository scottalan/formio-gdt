<script type="text/javascript">
<?php if ($name != 'new-preset'): ?>
  localStorage.setItem('formio-form-<?php print $name; ?>', JSON.stringify({
    action: '<?php print $callback; ?>'
  }));
<?php endif; ?>
(function b($, u) {
  if (typeof $.fn.seamless === u) {
    return setTimeout(b, 100);
  }
  $(function () {
    $('#formio-form-<?php print $name; ?>').seamless({fallback: false}).receive(function (event, submission) {
    });
  });
})(jQuery);
</script>
<iframe id="formio-form-<?php print $name; ?>" style="width:100%;border:none;" height="250px"
  src="/<?php print $module_path; ?>/view/src/view/index.html#/<?php print $project_hash; ?>/form/<?php print $_id; ?>?iframe=1<?php print $bootswatch_theme; ?>&header=0&name=<?php print $name; ?>">
</iframe>
