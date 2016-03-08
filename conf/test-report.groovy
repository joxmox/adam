import groovy.xml.XmlUtil

def outDir    = './build/docs/test'
def xmlDir    = "${outDir}/xml"
def namespace = 'adam'

// parse cmd args
args.each {arg ->
	if (arg.startsWith('-ns=')) {
		namespace = arg - '-ns='
	} else if (arg.startsWith('-dir=')) {
		outDir = arg - '-dir='
	} else {
		println 'usage: cat /path/to/gtest-results.xml | ./gtest-report.groovy [-dir=/path/to/outdir] [-ns=namespace]'
		System.exit 1
	}
}

// init
def ant = new AntBuilder()
ant.delete(dir:xmlDir)
ant.mkdir(dir:xmlDir)

// split testsuite nodes into separate XML files
// needed only because Ant JUnitReport doesn't understand the gtest XML file
// despite what's claim in their docs ;-/
def results = new XmlParser().parse(System.in) //load XML from stdin
results.testsuite.each { //foreach 'testsuite' node
	def name    = it.@name
	it.@package = namespace
	it.@name    = "${namespace}.${name}"

	def xmlFile  = new File("${xmlDir}/${name}.xml")
	xmlFile.text = XmlUtil.serialize(it) //emit XML to the file
}

// generate the HTML report, based on the XML files
ant.junitreport(todir:outDir) {
	fileset(dir:xmlDir) {
		include name:'*.xml'
	}
	report(format: 'noframes', todir:outDir)
}

// clean up
ant.delete(file:"${outDir}/TESTS-TestSuites.xml")
ant.delete(dir:xmlDir)
ant.move(file:"${outDir}/junit-noframes.html", tofile:"${outDir}/index.html")
ant.echo "written ${outDir}/test-results.html"

