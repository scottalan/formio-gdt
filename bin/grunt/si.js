module.exports = function(grunt) {

  grunt.loadNpmTasks('grunt-shell');

  // Remove existing libraries directory (it has nothing)
  grunt.config(['shell', 'siteinstall'], {
    command: [
      'cd build/html && drush si <%= config.si.profile %>',
      "--db-url=<%= config.si.db %>",
      "--account-name=<%= config.si.name %>",
      "--account-pass=<%= config.si.pass %>",
      "--db-su=<%= config.si.su %>",
      "--db-su-pw=<%= config.si.supw %>"
    ].join(' ')
  });

  grunt.registerTask('si', 'Installs a new site.', function() {
    grunt.task.run(['shell:siteinstall']);
  });

};
